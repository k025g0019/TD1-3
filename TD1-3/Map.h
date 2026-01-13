#pragma once

#define MAP_HEIGHT 45
#define MAP_WIDTH  180
#define LINE_BUF_SIZE 512

#define CHIP_W 16
#define CHIP_H 16
#define SHEET_COLS 8   // スプライトシートの横タイル数（要調整）

#define TILE_SIZE 16

int LoadMapCSV(const char* filePath, int map[MAP_HEIGHT][MAP_WIDTH]);
