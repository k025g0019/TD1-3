#pragma once
#include "Map.h"
enum MapType {
	MAP_EMPTY = -1,//空きマス（空欄的な感じ）

	MAP_WALL = 0,//壁いたってシンプル

	MAP_GOAL = -2,//ゴール地点これまたシンプル

	MAP_DANGER = -3,//危険地帯触れたらアウトのデンジャー

	MAP_BIRD = -4,

	MAP_DRONE = -5,

	MAP_WARPIN = -6,

	MAP_WARPOUT = -7,

	MAP_TRAMPOLINE = -8
};

const int kMapWidth = MAP_WIDTH;
const int kMapHeight = MAP_HEIGHT;
const int tileSize = 16;


float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);
#pragma once
