#pragma once
#include "Vector.h"
#include"Map.h"
class Player {
public:
	struct PlayerStatus {
		Vector2 pos;
		Vector2 vel;
		float radius = 25.0f;
	};

	PlayerStatus status;
	int Accelerated = 0;

    // ギミック判定用の関数を追加
    void CheckGimmicks(int map[MAP_HEIGHT][MAP_WIDTH]);

    void Initialize();
    void Update(int map[MAP_HEIGHT][MAP_WIDTH]);
    void Draw();

    bool CheckTileCollisions(int map[45][180]);

private:
	bool hitWall_ = false;
};
