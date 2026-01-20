#include <Novice.h>
#include "MapCollision.h"


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


// 右
bool MapCollisionBottom(float* x, float* y, float* r)
{
    int cx = int(*x / TILE_SIZE);
    int by = int((*y + *r) / TILE_SIZE);

    return !IsWall(cx, by);
}


// 上
bool MapCollisionLeft(float* x, float* y, float* r)
{
    int lx = int((*x - *r) / TILE_SIZE);
    int cy = int(*y / TILE_SIZE);

    return !IsWall(lx, cy);
}


// 下
bool MapCollisionRight(float* x, float* y, float* r)
{
    int rx = int((*x + *r) / TILE_SIZE);
    int cy = int(*y / TILE_SIZE);

    return !IsWall(rx, cy);
}
