#pragma once
#include <Novice.h>
#include "camera.h"
#define MAP_HEIGHT 45
#define MAP_WIDTH  180
#define LINE_BUF_SIZE 512

#define CHIP_W 16
#define CHIP_H 16
#define SHEET_COLS 8   // スプライトシートの横タイル数（要調整）
extern int gMap[MAP_HEIGHT][MAP_WIDTH];
#define TILE_SIZE 16
void itializeMap();

int LoadMapCSV(const char* filePath, int map[MAP_HEIGHT][MAP_WIDTH]);
void DrawMapChips(void);
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