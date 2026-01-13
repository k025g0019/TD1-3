#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "GameScene.h"

void Player::Initialize() {
	status.pos = { 640.0f, 60.0f };

	status.vel = { 6.5f, 0.5f }; // 初期速度

}

void Player::Update(int map[MAP_HEIGHT][MAP_WIDTH]) {

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

	status.pos.x += status.vel.x;
	status.pos.y += status.vel.y;

	// 壁との当たり判定（押し戻し）
	CheckTileCollisions(map);

	// ★ギミックとの判定（効果発動）
	CheckGimmicks(map);
}

void Player::CheckGimmicks(int map[MAP_HEIGHT][MAP_WIDTH])
{

	// プレイヤーの中心座標に対応するマップチップ番号を取得
	int cX = (int)(status.pos.x / TILE_SIZE);
	int cY = (int)(status.pos.y / TILE_SIZE);

	// 配列外参照防止
	if (cX < 0 || cX >= MAP_WIDTH || cY < 0 || cY >= MAP_HEIGHT) return;

	int tile = map[cY][cX];

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
		status.vel.y = -10.0f; // 強制的に上へ跳ねさせる
		break;

		// ▼ ワープ (In -> Out)
	case MAP_WARPIN:
		// マップ全体から出口(WARPOUT)を探して移動
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			for (int x = 0; x < MAP_WIDTH; x++)
			{
				if (map[y][x] == MAP_WARPOUT) {
					status.pos.x = (float)(x * TILE_SIZE) + status.radius;
					status.pos.y = (float)(y * TILE_SIZE) + status.radius;
					return; // 見つかったら即終了
				}
			}
		}
		break;

		// ▼ ドローン（ここが少し難しい）
		// ドローンは「上から踏んだか」「それ以外か」判定が必要なので
		// CheckTileCollisions の床判定の方に組み込むのが良いかもしれません。
		// ここでは簡易的に「触れたら減速」だけ書いておきます。
	case MAP_DRONE:
		status.vel.x *= 0.8f;
		break;
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
