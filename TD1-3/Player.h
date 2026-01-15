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
	struct PlayerStatus {
		Vector2 pos;
		Vector2 vel;
		float radius = 25.0f;
	};
	Vibration* vibration; // 破壊時振動
	PlayerStatus status;
	int Accelerated = 0;
	float sinParam[2] = { 0.5f, 0.5f }; // 待機中上下揺れ制御用パラメータ
	int times = 0; // 経過フレーム数
	void DoHitStop(int frames);

	bool isWarping_ = false;   // ワープ中か？
	Vector2 warpOffset_ = { 0, 0 }; // ワープ先との距離
	bool justWarped_ = false;
	Vector2 prevPos_ = { 0, 0 };

	WarpState warpState_ = WARP_NONE; // 現在の状態
	float warpTimer_ = 0.0f;          // アニメーション用タイマー
	Vector2 scale_ = { 1.0f, 1.0f };  // 見た目の倍率（1.0が通常）

	Vector2 warpStartPos_ = { 0, 0 }; // 吸い込まれる場所の中心
	Vector2 warpDestPos_ = { 0, 0 }; // 出てくる場所の中心

    // ギミック判定用の関数を追加
    void CheckGimmicks(int map[MAP_HEIGHT][MAP_WIDTH]);

	void UpdateWarpAnimation();

    void Initialize();
    void Update(int map[MAP_HEIGHT][MAP_WIDTH]);
    void Draw();

    bool CheckTileCollisions(int map[45][180]);

private:
	bool hitWall_ = false;
};
