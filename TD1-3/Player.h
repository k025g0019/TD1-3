#pragma once
#include "Vector.h"
#include "Map.h"
#include "HitStop.h"
#include "Vibration.h"

enum WarpState 
{
	WARP_NONE,
	WARP_IN,
	WARP_OUT
};
class Player {
public:
	struct PlayerStatus 
	{
		Vector2 pos = { 0.0f,0.0f };
		Vector2 vel = {0.0f,0.0f};
		float radius = 25.0f;
	};
	Vibration* vibration; // 破壊時振動
	PlayerStatus status;
	int Accelerated = 0;
	float sinParam[2] = { 0.5f, 0.5f }; // 待機中上下揺れ制御用パラメータ
	int times = 0; // 経過フレーム数
	void DoHitStop(int frames);

	bool justWarped_ = false;
	Vector2 prevPos_ = { 0, 0 };

	// 効果音ハンドル
	int playerHitMusic = -1;

	Player();
	~Player();

    // ギミック判定用の関数を追加
    void CheckGimmicks();

    void Initialize();
    void Update();
    void Draw();

    bool CheckTileCollisions();

private:
	bool hitWall_ = false;
};
