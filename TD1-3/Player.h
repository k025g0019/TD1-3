#pragma once
#include "Vector.h"
#include "Map.h"
#include "HitStop.h"
#include "Vibration.h"
class Player {
public:
	struct PlayerStatus
	{
		Vector2 pos = { 0.0f,0.0f };
		Vector2 vel = { 0.0f,0.0f };
		float radius = 25.0f;
	};
	bool isWarpCooldown_ = false;
	int  warpCooldownTimer_ = 0;

	bool isEasingActive;
	float easeFrame = 0.0f;
	float easeEndFrame = 100.0f;
	float easeTick = 0.0f;
	int JumpIndex = 5;
	int jumpTimer = 0;
	bool jumpAvailable = false;
	Vibration* vibration; // 破壊時振動
	PlayerStatus status;
	int Accelerated = 0;
	float moveDirX = 1.0f; // +1:右, -1:左
	float sinParam[2] = { 0.5f, 0.5f }; // 待機中上下揺れ制御用パラメータ
	int times = 0; // 経過フレーム数
	void Initialize();
	void Update();
	void Draw();
	void DoHitStop(int frames);
	float EaseInBounce(float t);
	bool CheckTileCollisions();
	Player();
	~Player();
private:
	bool hitWall_ = false;
};
