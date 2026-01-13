#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "GameScene.h"
#include "camera.h"
#include <cmath> // fabsf
void Player::Initialize() {
    status.pos = { 50.0f, 60.0f };
    status.vel = { 250.0f, 0.0f };   // px/s
    status.radius = 25.0f;
}


void Player::Update() {
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
}



void Player::Draw() {
	// プレイヤーを四角形で描画
	Novice::DrawEllipse(
		static_cast<int>(status.pos.x),
		static_cast<int>(status.pos.y),
		static_cast<int>(status.radius),
		static_cast<int>(status.radius),
		0.0f,
		0xFFFF00FF,
		kFillModeSolid
	);
}
bool Player::CheckTileCollisions(int map[MAP_HEIGHT][MAP_WIDTH]) {
	hitWall_ = false;

	if (!MapCollisionTop(&status.pos.x, &status.pos.y, &status.radius, map)) {
		int topIndex = int((status.pos.y - status.radius) / TILE_SIZE);
		status.pos.y = (topIndex + 1) * TILE_SIZE + status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionBottom(&status.pos.x, &status.pos.y, &status.radius, map)) {
		int bottomIndex = int((status.pos.y + status.radius) / TILE_SIZE);
		status.pos.y = bottomIndex * TILE_SIZE - status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionLeft(&status.pos.x, &status.pos.y, &status.radius, map)) {
		int leftIndex = int(status.pos.x / TILE_SIZE);
		status.pos.x = leftIndex * TILE_SIZE + status.radius;
		hitWall_ = true;
	}

	if (!MapCollisionRight(&status.pos.x, &status.pos.y, &status.radius, map)) {
		int rightIndex = int(status.pos.x / TILE_SIZE);
		status.pos.x = (rightIndex + 1) * TILE_SIZE - status.radius;
		hitWall_ = true;
	}

	return hitWall_;
}
