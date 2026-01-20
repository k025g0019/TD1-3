#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "camera.h"
#include <cmath> 
#include <math.h>
#include <algorithm>

int playerHitMusic = -1; // 初期値を設定

Player::Player()
{
    // 振動機能の初期化
    vibration = new Vibration(0);
}

Player::~Player()
{
    if (vibration) {
        delete vibration;
        vibration = nullptr;
    }
}

void Player::Initialize()
{
    status.pos = { 50.0f, 60.0f };
    status.vel = { 250.0f, 0.0f };   // px/s
    status.radius = 25.0f;
    justWarped_ = false; // ワープフラグ初期化

    if (playerHitMusic == -1)
    {
        playerHitMusic = Novice::LoadAudio("./Resource/Music/HitPlayer.mp3");
    }
}

void Player::DoHitStop(int frames) {
    HitStop::Instance().Start(frames);             // 時間停止
    Camera::Instance().StartShake(frames, 10.0f);   // 演出として画面揺れ
    Novice::PlayAudio(playerHitMusic, false, 1); // ヒット音再生
    if (vibration) vibration->runPattern(PATTERN_EXPLOSION_DAMAGE);
}

void Player::Update() {
    if (vibration) vibration->Update();
    const float dt = 1.0f / 60.0f;
    Camera::Instance().Follow(status.pos.x, status.pos.y);

    // 前フレームの座標を保存（判定用）
    prevPos_ = status.pos;

    // =========================
    // エアライダー寄りパラメータ
    // =========================
    const float gravity = 1000.0f;
    const float forwardAccel = 1200.0f;
    const float dragX = 1.8f;
    const float dragY = 1.2f;
    const float liftK = 3.2f;
    const float liftMaxRate = 0.99f;
    const float diveExtraDown = 900.0f;
    const float diveLiftRate = 0.35f;
    const float maxSinkGlide = 260.0f;
    const float maxSpeedX = 2000.0f;
    const float maxSpeedY = 1600.0f;

    // =========================
    // 入力
    // =========================
    const bool isDive = Novice::CheckHitKey(DIK_SPACE);

    // =========================
    // 加速度計算
    // =========================
    Vector2 acc = { 0.0f, 0.0f };

    acc.x += forwardAccel; // 前進
    acc.y += gravity;      // 重力

    // 揚力計算
    float speedX = fabsf(status.vel.x);
    float lift = liftK * speedX;
    float liftMax = gravity * liftMaxRate;
    if (lift > liftMax) lift = liftMax;

    if (isDive) {
        lift *= diveLiftRate;
        acc.y += diveExtraDown;
    }

    acc.y -= lift;

    // 空気抵抗
    acc.x += -dragX * status.vel.x;
    acc.y += -dragY * status.vel.y;

    // =========================
    // 速度更新
    // =========================
    status.vel.x += acc.x * dt;
    status.vel.y += acc.y * dt;

    // 速度制限
    if (status.vel.x > maxSpeedX) status.vel.x = maxSpeedX;
    if (status.vel.x < -maxSpeedX) status.vel.x = -maxSpeedX;
    if (status.vel.y > maxSpeedY) status.vel.y = maxSpeedY;
    if (status.vel.y < -maxSpeedY) status.vel.y = -maxSpeedY;

    if (!isDive) {
        if (status.vel.y > maxSinkGlide) status.vel.y = maxSinkGlide;
    }

    // 演出用
    times++;
    status.pos.y += sinf(times * sinParam[0]) * sinParam[1];

    // =========================
    // 位置更新
    // =========================
    status.pos.x += status.vel.x * dt;
    status.pos.y += status.vel.y * dt;

    // =========================
    // 画面端の反射
    // =========================
    const float halfSize = status.radius;
    const float screenWidth = 5000.0f;
    const float screenHeight = 720.0f;
    const float restitution = 0.55f;
    const float friction = 0.85f;

    if (status.pos.x <= halfSize) {
        status.pos.x = halfSize;
        status.vel.x = -status.vel.x * restitution;
        status.vel.y *= friction;
    }
    else if (status.pos.x >= screenWidth - halfSize) {
        status.pos.x = screenWidth - halfSize;
        status.vel.x = -status.vel.x * restitution;
        status.vel.y *= friction;
    }

    if (status.pos.y <= halfSize) {
        status.pos.y = halfSize;
        status.vel.y = -status.vel.y * restitution;
        status.vel.x *= friction;
    }
    else if (status.pos.y >= screenHeight - halfSize) {
        status.pos.y = screenHeight - halfSize;
        status.vel.y = -status.vel.y * restitution;
        status.vel.x *= friction;
    }

    // ★ ここにあった重複ワープ処理を削除しました。
    // ★ 下の CheckGimmicks でまとめて行います。

    // 壁との当たり判定（押し戻し）
    CheckTileCollisions();

    // ギミックとの判定（ワープ・敵など）
    CheckGimmicks();
}

// --------------------------------------------------------
// ギミック判定（修正版）
// プレイヤーの中心座標にあるタイルを判定します
// --------------------------------------------------------

void Player::CheckGimmicks()
{
    // 1. 今回の移動距離を計算
    float dx = status.pos.x - prevPos_.x;
    float dy = status.pos.y - prevPos_.y;
    float distance = sqrtf(dx * dx + dy * dy);

    // 2. チェックする細かさを決める（タイルの半分くらいの細かさで刻む）
    // どんなに速くても絶対にすり抜けないようにします
    float stepSize = TILE_SIZE / 2.0f;
    int steps = (int)(distance / stepSize);
    if (steps < 1) steps = 1; // 最低でも1回はチェック

    // 3. 通り道を少しずつ進みながらチェック
    for (int i = 0; i <= steps; i++)
    {
        // 調査する座標（線形補間：Lerp）
        float t = (float)i / (float)steps;
        float checkX = prevPos_.x + dx * t;
        float checkY = prevPos_.y + dy * t;

        // マス目の座標に変換
        int gx = (int)(checkX / TILE_SIZE);
        int gy = (int)(checkY / TILE_SIZE);

        // 配列外アクセス防止
        if (gx < 0 || gx >= MAP_WIDTH || gy < 0 || gy >= MAP_HEIGHT) continue;

        // 現在乗っているタイルのID
        int tile = gMap[gy][gx];

        // --- ワープ後の脱出判定 ---
        // ワープエリアから完全に離れたら、再ワープを許可する
        if (justWarped_)
        {
            if (tile != MAP_WARPIN && tile != MAP_WARPOUT)
            {
                justWarped_ = false;
            }
            // ワープ直後は、このフレームでの新たな判定を行わない（連続ワープ防止）
            continue;
        }

        // --- 各ギミックの判定 ---
        switch (tile)
        {
        case MAP_WARPIN: // ワープ入口 (5)
        {
            // 出口(6)を探す
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    if (gMap[y][x] == MAP_WARPOUT)
                    {
                        // 移動！
                        status.pos.x = (float)(x * TILE_SIZE) + (TILE_SIZE / 2.0f);
                        status.pos.y = (float)(y * TILE_SIZE) + (TILE_SIZE / 2.0f);

                        // 移動したので、このフレームの移動計算などはリセット
                        status.vel = { 0,0 }; // 勢いを殺す（必要なら）

                        // 無限ループ防止フラグをON
                        justWarped_ = true;

                        // ワープしたらこの関数の処理は即終了
                        return;
                    }
                }
            }
            break;
        }

        case MAP_DANGER:
            DoHitStop(30);
            Initialize();
            return; // 死んだら即終了

        case MAP_GOAL:
            break;

        case MAP_BIRD:
            status.vel.x *= 0.5f;
            status.vel.y *= 0.5f;
            break;

        case MAP_TRAMPOLINE: // (7)
            status.vel.y = -1200.0f;
            break;

        case MAP_DRONE:
            status.vel.x *= 0.8f;
            break;
        }
    }
}

void Player::Draw()
{
    Camera& cam = Camera::Instance();

    Novice::DrawEllipse(
        static_cast<int>(status.pos.x + cam.x),
        static_cast<int>(status.pos.y + cam.y),
        static_cast<int>(status.radius),
        static_cast<int>(status.radius),
        0.0f, WHITE, kFillModeSolid);
}

bool Player::CheckTileCollisions() {
    hitWall_ = false;

    // 壁判定関数が MapCollision.h / cpp で正しく定義されている前提
    if (!MapCollisionTop(&status.pos.x, &status.pos.y, &status.radius, gMap)) {
        int topIndex = int((status.pos.y - status.radius) / TILE_SIZE);
        status.pos.y = (topIndex + 1) * TILE_SIZE + status.radius;
        hitWall_ = true;
    }

    if (!MapCollisionBottom(&status.pos.x, &status.pos.y, &status.radius, gMap)) {
        int bottomIndex = int((status.pos.y + status.radius) / TILE_SIZE);
        status.pos.y = bottomIndex * TILE_SIZE - status.radius;
        hitWall_ = true;
    }

    if (!MapCollisionLeft(&status.pos.x, &status.pos.y, &status.radius, gMap)) {
        int leftIndex = int(status.pos.x / TILE_SIZE);
        status.pos.x = leftIndex * TILE_SIZE + status.radius;
        hitWall_ = true;
    }

    if (!MapCollisionRight(&status.pos.x, &status.pos.y, &status.radius, gMap)) {
        int rightIndex = int(status.pos.x / TILE_SIZE);
        status.pos.x = (rightIndex + 1) * TILE_SIZE - status.radius;
        hitWall_ = true;
    }

    return hitWall_;
}