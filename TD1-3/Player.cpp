#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"

#include "camera.h"
int playerHitMusic;
#include <cmath> // fabsf
#include <math.h>
#include<algorithm>
void Player::Initialize() {
	status.pos = { 50.0f, 60.0f };
	status.vel = { 250.0f, 0.0f };   // px/s
	status.radius = 25.0f;

}


void Player::DoHitStop(int frames) {

	HitStop::Instance().Start(frames);             // 時間停止
	Camera::Instance().StartShake(frames, 10.0f);   // 演出として画面揺れ
	Novice::PlayAudio(playerHitMusic, false, 1); // ヒット音再生


}

void Player::Update() {
	playerHitMusic = Novice::LoadAudio("./Resource/Music/HitPlayer.mp3");
	const float dt = 1.0f / 60.0f;
	Camera::Instance().Follow(status.pos.x, status.pos.y);
	// =========================
	// エアライダー寄りパラメータ
	// =========================
	// 重力（下向き）
	const float gravity = 1000.0f;          // px/s^2

	// 前進加速（速度は増えて良い：ただし抗力でだんだん効きが減る）
	const float forwardAccel = 1200.0f;     // px/s^2

	// 空気抵抗（X/Y別。Xは「無限加速」を抑える）
	const float dragX = 1.8f;               // 1/s
	const float dragY = 1.2f;               // 1/s

	// 速度から生まれる揚力（エアライド感の核）
	// lift = liftK * |vx| だと “速いほど沈みにくい”
	const float liftK = 3.2f;               // (px/s^2) / (px/s) = 1/s

	// 揚力の上限：上昇しないように重力の何割まで打ち消すか
	const float liftMaxRate = 0.99f;        // 0.90～0.98 推奨

	// ダイブ（Space）：揚力を弱め、沈みを増やして速度を稼ぐ
	const float diveExtraDown = 900.0f;     // px/s^2
	const float diveLiftRate = 0.35f;      // 0.20～0.60

	// “沈みすぎ破綻”防止（滑空中のみ）
	const float maxSinkGlide = 260.0f;      // px/s（200～350）

	// 速度上限（暴走防止：大きめでも可）
	const float maxSpeedX = 2000.0f;        // px/s
	const float maxSpeedY = 1600.0f;        // px/s

	// =========================
	// 入力
	// =========================
	const bool isDive = Novice::CheckHitKey(DIK_SPACE);

	// =========================
	// 加速度（エネルギー交換型）
	// =========================
	Vector2 acc = { 0.0f, 0.0f };

	// 前進（常時）
	acc.x += forwardAccel;

	// 重力（常時）
	acc.y += gravity;

	// 揚力：速いほど沈みにくい（ただし上昇しないよう上限）
	float speedX = fabsf(status.vel.x);
	float lift = liftK * speedX;

	float liftMax = gravity * liftMaxRate;
	if (lift > liftMax) lift = liftMax;

	// ダイブ中は揚力を弱めて沈みやすくする
	if (isDive) {
		lift *= diveLiftRate;
		acc.y += diveExtraDown;
	}

	// 揚力は上向きなのでマイナス
	acc.y -= lift;

	// 空気抵抗（速度に比例して逆向き）
	acc.x += -dragX * status.vel.x;
	acc.y += -dragY * status.vel.y;

	// =========================
	// 速度更新
	// =========================
	status.vel.x += acc.x * dt;
	status.vel.y += acc.y * dt;

	// 速度上限
	if (status.vel.x > maxSpeedX) status.vel.x = maxSpeedX;
	if (status.vel.x < -maxSpeedX) status.vel.x = -maxSpeedX;
	if (status.vel.y > maxSpeedY) status.vel.y = maxSpeedY;
	if (status.vel.y < -maxSpeedY) status.vel.y = -maxSpeedY;

	// 滑空中は沈下速度を制限（エアライドっぽい“伸び”）
	if (!isDive) {
		if (status.vel.y > maxSinkGlide) status.vel.y = maxSinkGlide;
	}
	times++;
	status.pos.y += sinf(times * sinParam[0]) * sinParam[1];
	// =========================
	// 位置更新
	// =========================
	status.pos.x += status.vel.x * dt;
	status.pos.y += status.vel.y * dt;

	// =========================
	// 壁反射（現実寄り：反発＋摩擦）
	// =========================
	const float halfSize = status.radius;
	const float screenWidth = 5000.0f;
	const float screenHeight = 720.0f;

	const float restitution = 0.55f;
	const float friction = 0.85f;

	// 左右
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

	// 上下
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
	// 壁との当たり判定（押し戻し）
	CheckTileCollisions();

	// ★ギミックとの判定（効果発動）
	CheckGimmicks();
}


void Player::CheckGimmicks()
{
    isWarping_ = false;

    // =========================================================
    // 1. 範囲総当たり判定 (Swept AABB) の準備
    // =========================================================
    float pLeft = prevPos_.x - status.radius;
    float pRight = prevPos_.x + status.radius;
    float pTop = prevPos_.y - status.radius;
    float pBottom = prevPos_.y + status.radius;

    float cLeft = status.pos.x - status.radius;
    float cRight = status.pos.x + status.radius;
    float cTop = status.pos.y - status.radius;
    float cBottom = status.pos.y + status.radius;

    int minX = (int)(fmin(pLeft, cLeft) / TILE_SIZE);
    int maxX = (int)(fmax(pRight, cRight) / TILE_SIZE);
    int minY = (int)(fmin(pTop, cTop) / TILE_SIZE);
    int maxY = (int)(fmax(pBottom, cBottom) / TILE_SIZE);

    if (minX < 0) minX = 0;
    if (maxX >= MAP_WIDTH) maxX = MAP_WIDTH - 1;
    if (minY < 0) minY = 0;
    if (maxY >= MAP_HEIGHT) maxY = MAP_HEIGHT - 1;

    // =========================================================
    // 2. ワープ後の「脱出待ち」チェック
    // =========================================================
    // ワープした直後なら、ワープマスから完全に離れるまで何もしない
    if (justWarped_) {
        bool stillTouchingWarp = false;

        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                int tile = map[y][x];
                if (tile == MAP_WARPIN || tile == MAP_WARPOUT) {
                    stillTouchingWarp = true;
                    break;
                }
            }
        }

        if (stillTouchingWarp) {
            return; // まだ重なっているので、他の判定もしない
        }
        else {
            justWarped_ = false; // 離れた！判定再開
        }
    }

    // =========================================================
    // 3. 全ギミック判定ループ
    // =========================================================
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {

            int tile = map[y][x];

            // -------------------------------------------------
            // ワープ処理 (MAP_WARPIN)
            // -------------------------------------------------
            if (tile == MAP_WARPIN) 
            {
                // 出口を探す
                int targetType = MAP_WARPOUT;
                for (int ty = 0; ty < MAP_HEIGHT; ty++) 
                {
                    for (int tx = 0; tx < MAP_WIDTH; tx++) 
                    {
                        if (map[ty][tx] == targetType) 
                        {

                            // 相手の中心座標
                            float targetCX = tx * TILE_SIZE + TILE_SIZE / 2.0f;
                            float targetCY = ty * TILE_SIZE + TILE_SIZE / 2.0f;

                            // 吸い込まれる入口の中心
                            float startCX = x * TILE_SIZE + TILE_SIZE / 2.0f;
                            float startCY = y * TILE_SIZE + TILE_SIZE / 2.0f;

                            // ▼▼▼ 即移動せず、演出セットアップ！ ▼▼▼

                            warpState_ = WARP_IN;     // 「吸い込みモード」へ
                            warpTimer_ = 0.0f;        // タイマーリセット
                            warpStartPos_ = { startCX, startCY }; // 入口の中心
                            warpDestPos_ = { targetCX, targetCY }; // 出口の中心

                            status.vel = { 0, 0 };    // 勢いを消す（重要）

                            return; // ここで処理を抜ける
                        }
                    }
                }
            }

            // -------------------------------------------------
            // その他のギミック処理
            // -------------------------------------------------
            switch (tile) {
            case MAP_DANGER:
                Initialize(); // 死んだらリセット
                return;       // 即終了

            case MAP_GOAL:
                // ゴール処理（シーン遷移フラグなどがあればここに書く）
                break;

            case MAP_BIRD:
                // 減速エリア
                status.vel.x *= 0.5f;
                status.vel.y *= 0.5f;
                break;

            case MAP_TRAMPOLINE:
                // 大ジャンプ
                status.vel.y = -1200.0f;
                break;

            case MAP_DRONE:
                // 横方向の減速
                status.vel.x *= 0.8f;
                break;
            }
        }
    }
}

// 線形補間（aからbへ tの割合で近づく）用の便利関数
float Lerp(float a, float b, float t) 
{
    return a + (b - a) * t;
}

void Player::UpdateWarpAnimation() 
{

    // アニメーションの速度（小さいほどゆっくり）
    const float kAnimSpeed = 1.0f / 30.0f; // 30フレームで完了

    warpTimer_ += kAnimSpeed;

    if (warpState_ == WARP_IN) {
        // =============================
        // 吸い込まれる動き (IN)
        // =============================

        // 1. 位置：現在地から「入口の中心」へズズズと動く
        // タイマーが進むにつれて warpStartPos に近づく
        status.pos.x = Lerp(status.pos.x, warpStartPos_.x, 0.2f);
        status.pos.y = Lerp(status.pos.y, warpStartPos_.y, 0.2f);

        // 2. 形：にゅいーん（横に細く、縦に長く）
        // タイマー(0.0 -> 1.0) に合わせて変形
        scale_.x = 1.0f - warpTimer_; // だんだん細くなる (1.0 -> 0.0)
        scale_.y = 1.0f + warpTimer_; // だんだん伸びる (1.0 -> 2.0)

        // 吸い込み完了？
        if (warpTimer_ >= 1.0f) {
            warpState_ = WARP_OUT; // 出るモードへ
            warpTimer_ = 0.0f;     // タイマーリセット

            // ★ここで瞬間移動！★
            status.pos = warpDestPos_;
        }

    }
    else if (warpState_ == WARP_OUT) {
        // =============================
        // 飛び出す動き (OUT)
        // =============================

        // 1. 形：細長い状態から、元の大きさに戻る
        // タイマー (0.0 -> 1.0)
        // x: 0.0 -> 1.0
        // y: 2.0 -> 1.0
        scale_.x = warpTimer_;
        scale_.y = 2.0f - warpTimer_;

        // 2. 位置：少し上に飛び出す感じを出したければ
        // status.pos.y -= 2.0f; // など足してもいい

        // 完了？
        if (warpTimer_ >= 1.0f) {
            warpState_ = WARP_NONE; // 通常モードへ戻る
            scale_ = { 1.0f, 1.0f }; // 大きさを完璧に戻す

            // 出た瞬間に少しジャンプさせる？（お好みで）
            status.vel.y = -300.0f;
        }
    }
}

void Player::Draw() 
{
    // 描画する幅と高さを、scale_ 倍する
    float drawW = status.radius * 2 * scale_.x;
    float drawH = status.radius * 2 * scale_.y;

    // 中心基準で描画するために座標を調整
    // (pos は中心座標と仮定)
    float drawX = status.pos.x - (drawW / 2.0f);
    float drawY = status.pos.y - (drawH / 2.0f);

    // 描画 (色は白とか適当に)
    Novice::DrawBox(
        (int)drawX, (int)drawY,
        (int)drawW, (int)drawH,
        0.0f, WHITE, kFillModeSolid);
}

bool Player::CheckTileCollisions(int map[MAP_HEIGHT][MAP_WIDTH]) {
	hitWall_ = false;

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
