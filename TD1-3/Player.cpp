#include "Player.h"
#include <Novice.h>
#include "Map.h"
#include "MapCollision.h"
#include "camera.h"
int playerHitMusic = -1;
#include <cmath> // fabsf

float  Player::EaseInBounce(float t) {
	return t * t * (3.0f - 2.0f * t);
}
Player::Player()
{
	// 振動機能の初期化
	vibration = new Vibration(0);
}
bool CircleRectHit(
	float cx, float cy, float cr,
	float rx, float ry, float rw, float rh
)
{
	// 円中心から矩形への最近点
	float nearestX = fmaxf(rx, fminf(cx, rx + rw));
	float nearestY = fmaxf(ry, fminf(cy, ry + rh));

	float dx = cx - nearestX;
	float dy = cy - nearestY;

	return (dx * dx + dy * dy) <= (cr * cr);
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
	status.radius = 15.0f;
	JumpIndex = 5;
	jumpTimer = 0;
	jumpAvailable = false;
	easeEndFrame = 240.0f;
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

	if (isWarpCooldown_) {
		warpCooldownTimer_--;
		if (warpCooldownTimer_ <= 0) {
			isWarpCooldown_ = false;
		}
	}


	// =========================
	// エアライダー寄りパラメータ
	// =========================
	const float gravity = 800.0f;
	const float forwardAccel = 500.0f;
	const float dragX = 2.2f;
	const float dragY = 1.6f;
	const float liftK = 3.2f;
	const float liftMaxRate = 0.99f;
	const float diveExtraDown = 900.0f;
	const float diveLiftRate = 0.75f;
	const float maxSinkGlide = 200.0f;
	const float maxSpeedX = 550.0f;
	const float maxSpeedY = 1600.0f;

	// =========================
	// 入力
	// =========================

	const bool isDive = Novice::CheckHitKey(DIK_SPACE);


	if (jumpAvailable) {
		jumpTimer++;
		if (JumpIndex <= jumpTimer) {
			jumpAvailable = false;
		}
	}


	// =========================
	// 加速度計算
	// =========================
	Vector2 acc = { 0.0f, 0.0f };

	acc.x += forwardAccel * moveDirX; // 前進
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

	// 速度上限
	if (status.vel.x > maxSpeedX) status.vel.x = maxSpeedX;
	if (status.vel.x < -maxSpeedX) status.vel.x = -maxSpeedX;
	if (status.vel.y > maxSpeedY) status.vel.y = maxSpeedY;
	if (status.vel.y < -maxSpeedY) status.vel.y = -maxSpeedY;

	// 滑空中は沈下速度を制限（エアライドっぽい“伸び”）
	if (!isDive && !jumpAvailable) {
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
	for (int i = 0; i < gEntityCount; i++) {
		if (gEntities[i].types == ENTITY_OpenSesame) {
			if (isEasingActive) {
				easeFrame += 1.0f;
				if (easeFrame >= easeEndFrame) {
					isEasingActive = false;
					easeFrame = easeEndFrame;
				}
				float t = easeFrame / easeEndFrame;
				float easeValue = EaseInBounce(t);
				gEntities[i].y = (int)(gEntities[i].y + (200.0f * easeValue));
			}
		}

	}
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

	for (int i = 0; i < gEntityCount; i++)
	{
		// Entity 矩形
		float ex = (float)gEntities[i].x - gEntities[i].w / 2;
		float ey = (float)gEntities[i].y - gEntities[i].h / 2;
		float ew = (float)gEntities[i].w;
		float eh = (float)gEntities[i].h;

		// プレイヤー足元
		float playerBottom = status.pos.y + status.radius;

		// Entity 上面
		float entityTop = ey;
		float nearestX = fmaxf(ex, fminf(status.pos.x, ex + ew));
		float nearestY = fmaxf(ey, fminf(status.pos.y, ey + eh));

		float dx = status.pos.x - nearestX;
		float dy = status.pos.y - nearestY;

		// ▼ 円 vs 矩形
		if (CircleRectHit(
			status.pos.x,
			status.pos.y,
			status.radius,
			ex, ey, ew, eh
		))
		{
			switch (gEntities[i].types)
			{
			case ENTITY_Entity:
				// トランポリン
				// ▼ 上から踏んだときだけ
				if (status.vel.y > 0.0f && playerBottom < entityTop + 10.0f)
				{
					// トランポリン反発
					status.vel.y = -fabsf(status.vel.y) * 1.2f - 300.0f;

					// めり込み防止（少し上に戻す）
					status.pos.y = entityTop - status.radius;
					jumpAvailable = true;
					// ヒット演出
					DoHitStop(6);
				}
				break;

			case ENTITY_Trampoline_R:
				if (status.vel.y > 0.0f && playerBottom < entityTop + 10.0f)
				{
					status.vel.y = -fabsf(status.vel.y) * 1.2f - 300.0f;
					moveDirX *= -1.0f;
					jumpAvailable = true;
					// めり込み防止（少し上に戻す）
					status.pos.y = entityTop - status.radius;
					status.vel.x = -fabsf(status.vel.x); // 右へ
					// ヒット演出
					DoHitStop(6);
				}
				break;
			case ENTITY_Trampoline_L:

				if (status.vel.y > 0.0f && playerBottom < entityTop + 10.0f)
				{
					status.vel.y = -fabsf(status.vel.y) * 1.2f - 300.0f;
					moveDirX *= -1.0f;
					jumpAvailable = true;
					// めり込み防止（少し上に戻す）
					status.pos.y = entityTop - status.radius;
					status.vel.x = fabsf(status.vel.x); // 右へ
					// ヒット演出
					DoHitStop(6);
				}
				break;
			case ENTITY_SWITCHR:
				// 右方向へ進行中のみ
				if (moveDirX == -1.0f)
				{
					// 横から接触していて、かつスイッチの右側から当たった
					if (fabsf(dx) > fabsf(dy) && dx > 0.0f)
					{
						// めり込み防止
						status.pos.x = ex + ew + status.radius;

						// 進行方向反転
						moveDirX = 1.0f;
						status.vel.x = fabsf(status.vel.x); // 右へ
						DoHitStop(4);
						if (!isEasingActive) {
							isEasingActive = true;
							easeFrame = 0.0f;
						}
					}
				}

				break;
			case ENTITY_OpenSesame:

				status.pos.x = ex - (ew / 2) + status.radius;
				break;
			case ENTITY_BREAKSWALL:
				if (status.vel.y > 100.0f && playerBottom < entityTop + 10.0f)
				{
					status.vel.y -= 100;
					// 壁破壊演出
    					DoHitStop(8);
					// 壁を消す
					gEntities[i].y = -1000; // 画面外へ移動させるなど
				}
				break;
			case ENTITY_Drawmn:
				// ▼ 上から踏んだときだけ
				if (status.vel.y > 0.0f && playerBottom < entityTop + 10.0f)
				{
					// トランポリン反発
					status.vel.y = -fabsf(status.vel.y) * 1.2f - 300.0f;

					// めり込み防止（少し上に戻す）
					status.pos.y = entityTop - status.radius;
					jumpAvailable = true;
					// ヒット演出
					DoHitStop(6);
				}
				break;
			case ENTITY_WARP:
			{
				// クールタイム中は無視
				if (isWarpCooldown_) break;

				int srcWarpId = gEntities[i].warpId;
				if (srcWarpId < 0) break;

				// 同じ warpId の別 Warp を探す
				for (int j = 0; j < gEntityCount; j++)
				{
					if (i == j) continue;

					if (gEntities[j].types != ENTITY_WARP) continue;
					if (gEntities[j].warpId != srcWarpId) continue;

					// ワープ実行
					status.pos.x = (float)gEntities[j].x;
					status.pos.y = (float)gEntities[j].y;



					// 無限往復防止
					isWarpCooldown_ = true;
					warpCooldownTimer_ = 30; // 0.5 秒


					return;
				}
			}
			break;

			default:
				break;
			}

		}



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
}
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
