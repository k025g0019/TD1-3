#include <Novice.h>
#include "MapCollision.h"
#include "Map.h"

// 左方向の当たり判定
float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]) {
    int left = static_cast<int>((*playerPosX - *radius - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

    if (left >= 0 &&
        top >= 0 &&
        bottom < y &&
        mapDeta[top][left] <=1 &&
        mapDeta[bottom][left] <=1) {
        return true;
    }
    else {
        return false;
    }
}

// 右方向の当たり判定
float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]) {
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

    if (right < x &&
        top >= 0 &&
        bottom < y &&
        mapDeta[top][right] <=1 &&
        mapDeta[bottom][right] <=1) {
        return true;
    }
    else {
        return false;
    }
}

// 上方向の当たり判定
float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);

    if (top >= 0 &&
        left >= 0 &&
        right < x &&
        mapDeta[top][left] <=1 &&
        mapDeta[top][right] <=1) {
        return true;
    }
    else {
        return false;
    }
}

// 下方向の当たり判定
float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 + 1) / tileSize);

    if (bottom < y &&
        left >= 0 &&
        right < x &&
        mapDeta[bottom][left] <=1 &&
        mapDeta[bottom][right] <=1) {
        return true;
    }
    else {
        return false;
    }
}
