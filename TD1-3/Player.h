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
	enum class PoseState {
		Horizontal,     // 横（通常）
		ToVertical,     // 横 → 縦 へ遷移中
		Vertical,       // 縦で固定
		ToHorizontal    // 縦 → 横 へ戻り中
	};
	PoseState poseState_ = PoseState::Horizontal;
	int poseFrame_ = 0;              // 見た目用フレーム
	int poseFrameTimer_ = 0;

	int walkFrame_ = 0;
	int walkFrameTimer_ = 0;
	bool isWarpCooldown_ = false;
	int  warpCooldownTimer_ = 0;
	// ワープ演出用
	bool isWarping_ = false;        // 演出中
	bool isCloneActive_ = false;    // 入口残像を描画するか
	bool isDead_ = false;
	bool IsDead() const { return isDead_; }

	Vector2 warpClonePos_;          // 入口側の残像（複製）位置
	int warpSrcIndex_ = -1;         // 入口warp
	int warpDstIndex_ = -1;         // 出口warp

	bool isGrounded_ = false;
	bool isGameOver_ = false;

	bool hitTrampoline = false;
	int PlayerImage = -1;
	int PlayerflallImage = -1;
	int PlayerImageFrame = 0;
	int PlayerImageMaxFrame = 4;
	int PlayerImageFrameTimer = 0;
	bool isDiveActive = false;
	bool fallActive = false;
	bool isEasingActive;
	float easeFrame = 0.0f;
	float easeEndFrame = 100.0f;
	float easeTick = 0.0f;
	int JumpIndex = 5;
	int jumpTimer = 0;
	bool jumpAvailable = false;
	void UpdeteLeftJoystik();
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
