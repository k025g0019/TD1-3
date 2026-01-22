#pragma once
#include <Novice.h>
#include "camera.h"

#define MAP_HEIGHT 45
#define MAP_WIDTH  180

#define CHIP_W 16
#define CHIP_H 16
#define SHEET_COLS 20
#define SHEET_ROWS 15
#define TILE_SIZE 16

#define MAX_ENTITIES 128

enum EntityType
{
    ENTITY_Entity,
    ENTITY_Trampoline_R,
    ENTITY_Trampoline_L,
    ENTITY_UNKNOWN,
    ENTITY_SWITCHR,
    ENTITY_OpenSesame,
    ENTITY_BREAKSWALL,
    ENTITY_Drawmn,
    ENTITY_WARP        
};

typedef struct Entity
{
    char name[32];
    EntityType types; 
    int x, y;
    int w, h;
    int startX, startY;
    int endX, endY;
    float timer;
    float easeFrame;
    int warpId;
} Entity;

// ★ ヘッダは extern のみ
extern Entity gEntities[MAX_ENTITIES];
extern int gEntityCount;

extern int gMap[MAP_HEIGHT][MAP_WIDTH];
extern int gVisualMap[MAP_HEIGHT][MAP_WIDTH];
extern int gCollisionMap[MAP_HEIGHT][MAP_WIDTH];
extern int gChipSheetHandle;

void InitializeMap();
int  LoadMapLDtk(const char* filePath);
void DrawMapChips(void);
void DrawEntities(void);

void UpdateEntity();
enum MapType {
    MAP_EMPTY = 0,
    MAP_WALL = 1,
    MAP_GOAL = 2,
    MAP_DANGER = 3,
    MAP_BIRD = 4,
    MAP_DRONE = 5,
    MAP_WARPIN = 6,
    MAP_WARPOUT = 7,
    MAP_TRAMPOLINE = 8
};
