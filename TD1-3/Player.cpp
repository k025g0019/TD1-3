#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "GameScene.h"

void Player::Initialize() {
	status.pos = { 640.0f, 60.0f };

	status.vel = { 6.5f, 0.5f }; // 初期速度

}

void Player::Update() {

	// 入力による速度制御
	if (Novice::CheckHitKey(DIK_SPACE)) {
		// 降下
		status.vel.y = 5.5f;
	}
	else {
		// 滑空
		status.vel.y = 0.5f;
	}

	// X方向は常に進む（反射で符号だけ変える）
	// ※ 初期値は Initialize で設定する前提

	// 位置更新
	status.pos.x += status.vel.x;
	status.pos.y += status.vel.y;

	// ---- 壁反射処理 ----
	const float halfSize = 25.0f;
	const float screenWidth = 1280.0f;
	const float screenHeight = 720.0f;

	// 左右の壁
	if (status.pos.x <= halfSize) {
		status.pos.x = halfSize;
		status.vel.x *= -1;
	}
	else if (status.pos.x >= screenWidth - halfSize) {
		status.pos.x = screenWidth - halfSize;
		status.vel.x *= -1;
	}

	// 上下の壁（必要なら）
	if (status.pos.y <= halfSize) {
		status.pos.y = halfSize;
		status.vel.y *= -1;
	}
	else if (status.pos.y >= screenHeight - halfSize) {
		status.pos.y = screenHeight - halfSize;
		status.vel.y *= -1;
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
