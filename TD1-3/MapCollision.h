#pragma once
#include "Map.h"
enum MapType {
	MAP_EMPTY = -1,//空きマス（空欄的な感じ）

	MAP_WALL = 0,//壁いたってシンプル

	MAP_GOAL = 2,//ゴール地点これまたシンプル

	MAP_DANGER = 3//危険地帯触れたらアウトのデンジャー
};

const int x = MAP_WIDTH;
const int y = MAP_HEIGHT;
const int tileSize = 16;


float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]);

float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]);

float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]);

float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[y][x]);
#pragma once
