#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"


int gMap[MAP_HEIGHT][MAP_WIDTH] = {};
int gChipSheetHandle = -1;
int drawnHandle = -1;

void itializeMap()
{
	gChipSheetHandle = Novice::LoadTexture("./Resource/Image/sand.png");
	drawnHandle = Novice::LoadTexture("./Resource/Image/Drawn.bmp");
}
int LoadMapCSV(const char* filePath, int map[MAP_HEIGHT][MAP_WIDTH])
{
    FILE* fp = NULL;

    if (fopen_s(&fp, filePath, "r") != 0 || fp == NULL) {
        return 0;
    }

    char line[LINE_BUF_SIZE];
    int y = 0;

    while (fgets(line, (int)sizeof(line), fp) != NULL && y < MAP_HEIGHT) {
        line[strcspn(line, "\r\n")] = '\0';

        int x = 0;
        char* context = NULL;
        char* token = strtok_s(line, ",", &context);

        while (token != NULL && x < MAP_WIDTH) {
            map[y][x] = atoi(token);
            token = strtok_s(NULL, ",", &context);
            x++;
        }


        while (x < MAP_WIDTH) {
            map[y][x] = -1;
            x++;
        }

        y++;
    }


    while (y < MAP_HEIGHT) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = -1;
        }
        y++;
    }

    fclose(fp);
    return 1;
}

void DrawMapChips(void)
{

	Camera& cam = Camera::Instance();
	// マップ描画ループの中身
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			int id = gMap[y][x];

			// CSV の値 = タイルシートのインデックス
			int chipIndex = id;

			int srcX = (chipIndex % SHEET_COLS) * CHIP_W;
			int srcY = (chipIndex / SHEET_COLS) * CHIP_H;

			int dstX = x * TILE_SIZE + (int)cam.x;
			int dstY = y * TILE_SIZE + (int)cam.y;

			// enum を使って分岐
			switch (id)
			{

				// ■ 壁の描画
			case MAP_WALL:
				Novice::DrawSpriteRect(
					dstX, dstY,
					srcX, srcY,
					CHIP_W, CHIP_H,
					gChipSheetHandle,
					1.0f, 1.0f,
					0.0f,
					0xFFFFFFFF
				);
				break;

				// ■ ゴールの描画
			case MAP_GOAL:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0x00FF00FF, // 緑
					kFillModeSolid
				);
				break;

				// ■ 危険地帯（トゲなど）の描画
			case MAP_DANGER:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0xFF0000FF, // 赤
					kFillModeSolid
				);
				break;
			case MAP_BIRD:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0xFF0000FF, // 赤
					kFillModeSolid
				);
				break;
			case MAP_DRONE:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0xFF0000FF, // 赤
					kFillModeSolid
				);
				break;
			case MAP_WARPIN:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					BLACK,
					kFillModeSolid
				);
				break;
			case MAP_WARPOUT:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					BLACK,
					kFillModeSolid
				);
				break;
			case MAP_TRAMPOLINE:
				Novice::DrawBox(
					dstX, dstY,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0xFF0000FF, // 赤
					kFillModeSolid
				);
				break;
				// □ 空きマス (MAP_EMPTY = -1) や未定義の値
			case MAP_EMPTY:
			default:
				// 何も描画しない
				break;
			}
		}
	}
}