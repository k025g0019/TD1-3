#pragma once
#include "Map.h"


const int kMapWidth = MAP_WIDTH;
const int kMapHeight = MAP_HEIGHT;
const int tileSize = 16;

float MapCollisionLeft(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]);

float MapCollisionRight(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]);

float MapCollisionTop(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]);

float MapCollisionBottom(float* playerPosX, float* playerPosY, float* radius, int mapDeta[MAP_HEIGHT][MAP_WIDTH]);