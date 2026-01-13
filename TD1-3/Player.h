#pragma once
#include "Vector.h"
class Player {
public:
    struct PlayerStatus {
        Vector2 pos;
        Vector2 vel;
        float radius = 25.0f;
    };

    PlayerStatus status;

    void Initialize();
    void Update();
    void Draw();

    bool CheckTileCollisions(int map[45][180]);

private:
    bool hitWall_ = false;
};
