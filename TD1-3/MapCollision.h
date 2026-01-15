#pragma once
#include "Map.h"


const int kMapWidth = MAP_WIDTH;
const int kMapHeight = MAP_HEIGHT;
const int tileSize = 16;


float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);

float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[kMapHeight][kMapWidth]);
#pragma once
