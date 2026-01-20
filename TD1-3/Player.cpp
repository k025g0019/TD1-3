#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "camera.h"
int playerHitMusic;
#include <cmath> // fabsf
void Player::Initialize() {
	status.pos = { 50.0f, 60.0f };
	status.vel = { 250.0f, 0.0f };   // px/s
	status.radius = 25.0f;

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
	if (CheckTileCollisions()) {
		DoHitStop(10);  // ← 6フレームだけ時間停止
	}

	// ★ギミックとの判定（効果発動）
	CheckGimmicks();
}

// --------------------------------------------------------
// ギミック判定（修正版）
// プレイヤーの中心座標にあるタイルを判定します
// --------------------------------------------------------

void Player::CheckGimmicks()
{

	// プレイヤーの中心座標に対応するマップチップ番号を取得
	int cX = (int)(status.pos.x / TILE_SIZE);
	int cY = (int)(status.pos.y / TILE_SIZE);

	// 配列外参照防止
	if (cX < 0 || cX >= MAP_WIDTH || cY < 0 || cY >= MAP_HEIGHT) return;

	int tile = gCollisionMap[cY][cX];

	switch (tile)
	{
		// ▼ 危険地帯
	case MAP_DANGER:
		// ミス処理へ（初期位置に戻すなど）
		// Initialize();
		break;

		// ▼ ゴール
	case MAP_GOAL:
		// シーン遷移フラグを立てるなど
		break;

		// ▼ 鳥（減速）
	case MAP_BIRD:
		status.vel.x *= 0.5f; // 速度を半分にする
		status.vel.y *= 0.5f;
		break;

		// ▼ トランポリン
	case MAP_TRAMPOLINE:
		status.vel.y = -200.0f; // 強制的に上へ跳ねさせる
		break;


		// ▼ ワープ (In -> Out)
	case MAP_WARPIN:
		// マップ全体から出口(WARPOUT)を探して移動
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (gCollisionMap[y][x] == MAP_WARPOUT) {
					status.pos.x = (float)(x * TILE_SIZE) + status.radius;
					status.pos.y = (float)(y * TILE_SIZE) + status.radius;
					return; // 見つかったら即終了
				}
			}
		}
		break;
		// ▼ ドローン
	case MAP_DRONE:
		status.vel.x *= 0.8f;
		break;
	}
}
void Player::Draw() {
	Camera& cam = Camera::Instance();
	Novice::DrawEllipse(
		static_cast<int>(status.pos.x + cam.x),
		static_cast<int>(status.pos.y + cam.y),
		static_cast<int>(status.radius),
		static_cast<int>(status.radius),
		0.0f,
		0xFFFF00FF,
		kFillModeSolid
	);

bool Player::CheckTileCollisions()
{
	hitWall_ = false;

	if (!MapCollisionTop(&status.pos.x, &status.pos.y, &status.radius))
	{
		int ty = int((status.pos.y - status.radius) / TILE_SIZE);
		status.pos.y = (ty + 1) * TILE_SIZE + status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionBottom(&status.pos.x, &status.pos.y, &status.radius))
	{
		int by = int((status.pos.y + status.radius) / TILE_SIZE);
		status.pos.y = by * TILE_SIZE - status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionLeft(&status.pos.x, &status.pos.y, &status.radius))
	{
		int lx = int((status.pos.x - status.radius) / TILE_SIZE);
		status.pos.x = (lx + 1) * TILE_SIZE + status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionRight(&status.pos.x, &status.pos.y, &status.radius))
	{
		int rx = int((status.pos.x + status.radius) / TILE_SIZE);
		status.pos.x = rx * TILE_SIZE - status.radius;
		hitWall_ = true;
	}

	return hitWall_;
}
