#include <Novice.h>
#include "MapCollision.h"
#include "Map.h"

// 左方向の当たり判定
float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]) {
    int left = static_cast<int>((*playerPosX - *radius - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

    if (left >= 0 &&
        top >= 0 &&
        bottom < MAP_HEIGHT &&
        mapDeta[top][left] != MAP_WALL &&
        mapDeta[bottom][left] != MAP_WALL) {
        return true;
    }
    else {
        return false;
    }
}

// 右方向の当たり判定
float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]) {
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

    if (right < MAP_WIDTH &&
        top >= 0 &&
        bottom < MAP_HEIGHT &&
        mapDeta[top][right] != MAP_WALL &&
        mapDeta[bottom][right] != MAP_WALL) {
        return true;
    }
    else {
        return false;
    }
}

// 上方向の当たり判定
float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);

    if (top >= 0 &&
        left >= 0 &&
        right < MAP_WIDTH &&
        mapDeta[top][left] != MAP_WALL && 
        mapDeta[top][right] != MAP_WALL) {
        return true;
    }
    else {
        return false;
    }
}

// 下方向の当たり判定
float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 + 1) / tileSize);

    if (bottom < MAP_HEIGHT &&
        left >= 0 &&
        right < MAP_WIDTH &&
        mapDeta[bottom][left] != MAP_WALL &&
        mapDeta[bottom][right] != MAP_WALL) {
        return true;
    }
    else {
        return false;
    }
}