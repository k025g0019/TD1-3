#pragma once
#include "Map.h"


const int kMapWidth = MAP_WIDTH;
const int kMapHeight = MAP_HEIGHT;
const int tileSize = 16;


bool MapCollisionTop(float* x, float* y, float* r);
bool MapCollisionBottom(float* x, float* y, float* r);

bool MapCollisionLeft(float* x, float* y, float* r);

bool MapCollisionRight(float* x, float* y, float* r);
#pragma once
