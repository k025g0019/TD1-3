#include <Novice.h>
#include "MapCollision.h"

// 左方向の当たり判定
float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]) {
    int left = static_cast<int>((*playerPosX - *radius - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

// 壁かどうか
inline bool IsWall(int x, int y)
{
    if (x < 0 || x >= MAP_WIDTH ||
        y < 0 || y >= MAP_HEIGHT)
    {
        return true; // 画面外は壁
    }
    return gCollisionMap[y][x] == 1;
}

// 左
bool MapCollisionTop(float* x, float* y, float* r)
{
    int cx = int(*x / TILE_SIZE);
    int ty = int((*y - *r) / TILE_SIZE);

    return !IsWall(cx, ty);
}

// 右方向の当たり判定
float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]) {
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 - 1) / tileSize);

// 右
bool MapCollisionBottom(float* x, float* y, float* r)
{
    int cx = int(*x / TILE_SIZE);
    int by = int((*y + *r) / TILE_SIZE);

    return !IsWall(cx, by);
}
}
// 上方向の当たり判定
float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
    int top = static_cast<int>((*playerPosY - *radius) / tileSize);
// 上
bool MapCollisionLeft(float* x, float* y, float* r)
{
    int lx = int((*x - *r) / TILE_SIZE);
    int cy = int(*y / TILE_SIZE);

    return !IsWall(lx, cy);
}
}
// 下方向の当たり判定
float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]) {
    int left = static_cast<int>((*playerPosX - *radius) / tileSize);
    int right = static_cast<int>(((*playerPosX - *radius) + *radius * 2 - 1) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 + 1) / tileSize);
    int bottom = static_cast<int>(((*playerPosY - *radius) + *radius * 2 + 1) / tileSize);
// 下
bool MapCollisionRight(float* x, float* y, float* r)
{
    int rx = int((*x + *r) / TILE_SIZE);
    int cy = int(*y / TILE_SIZE);

    return !IsWall(rx, cy);
}
}