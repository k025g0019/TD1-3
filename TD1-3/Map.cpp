#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Map.h"

#include "cJSON.h"
#include <math.h>
#define M_PI 3.1415926535897932384626433832795028841971f
int gMap[MAP_HEIGHT][MAP_WIDTH];
int Number = 0;
Entity gEntities[MAX_ENTITIES];
int gEntityCount = 0;
int gEnemyTex = -1;
int gPlayerTex = -1;
int DrawnTex = -1;
int TranporinTex = -1;
int TranporinJumpTex = -1;

int TranporinRTex = -1;
int TranporinRjumpTex = -1;
int TranporinLTex = -1;
int TranporinLjumpTex = -1;
int SwitchTex = -1;
int SwitchPusshTex = -1;
int warpTex = -1;

int walkFrameTimer_[5] = { 1 };
int walkFrame_[5] = { 0 };
int maxFrame[5] = { 59,59,4,4,4 };
// ============================
// グローバル
// ============================
int gVisualMap[MAP_HEIGHT][MAP_WIDTH];
int gCollisionMap[MAP_HEIGHT][MAP_WIDTH];
int gChipSheetHandle = -1;


TrampolineAnimState gTrampolineAnim[MAX_ENTITIES];


TrampolineAnimState gTrampolineAnimR[MAX_ENTITIES];


TrampolineAnimState gTrampolineAnimL[MAX_ENTITIES];
SwitchState switchState;
float easeInOutElastic(float t) {
	return -(cosf(M_PI * t) - 1) / 2;

}
// ============================
// タイルシート最大数
// ============================
static inline int GetSheetMaxTiles()
{
	return SHEET_COLS * SHEET_ROWS;
}

// ============================
// 初期化
// ============================
void InitializeMap()
{
	gChipSheetHandle =
		Novice::LoadTexture("./Resource/Image/Inca_front_by_Kronbits-extended.png");
	gPlayerTex = Novice::LoadTexture("./Resource/Image/pDrawn.bmp");
	gEnemyTex = Novice::LoadTexture("./Resource/Image/Drawn.bmp");
	DrawnTex = Novice::LoadTexture("./Resource/Image/Drawn.png");
	TranporinTex = Novice::LoadTexture("./Resource/Image/Tranporin.png");
	TranporinJumpTex = Novice::LoadTexture("./Resource/Image/Tranporinjump.png");
	SwitchTex = Novice::LoadTexture("./Resource/Image/swici.png");
	SwitchPusshTex = Novice::LoadTexture("./Resource/Image/swiciPuss.png");
	warpTex = Novice::LoadTexture("./Resource/Image/warp.png");
	TranporinRTex = Novice::LoadTexture("./Resource/Image/TranporinRight.png");
	TranporinRjumpTex = Novice::LoadTexture("./Resource/Image/TranporinjumpRight.png");
	TranporinLTex = Novice::LoadTexture("./Resource/Image/TranporinLeft.png");
	TranporinLjumpTex = Novice::LoadTexture("./Resource/Image/TranporinjumpLeft.png");

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			gVisualMap[y][x] = -1;
			gCollisionMap[y][x] = MAP_EMPTY;
		}
	}

	for (int i = 0; i < gEntityCount; i++) {
		gTrampolineAnim[i].isPlaying = false;
		gTrampolineAnim[i].frame = 0;
	}

	gEntityCount = 0;

}

// ============================
// LDtk 読み込み
// ============================
int LoadMapLDtk(const char* filePath, int levelIndex)
{
	FILE* fp = NULL;
	if (fopen_s(&fp, filePath, "rb") != 0 || !fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* buffer = (char*)malloc(size + 1);
	fread(buffer, 1, size, fp);
	buffer[size] = '\0';
	fclose(fp);

	cJSON* root = cJSON_Parse(buffer);
	free(buffer);
	if (!root) return 0;

	cJSON* levels = cJSON_GetObjectItem(root, "levels");
	int levelCount = cJSON_GetArraySize(levels);

	if (levelIndex < 0 || levelIndex >= levelCount) {
		cJSON_Delete(root);
		return 0;
	}

	cJSON* level = cJSON_GetArrayItem(levels, levelIndex);
	cJSON* layers = cJSON_GetObjectItem(level, "layerInstances");
	int layerCount = cJSON_GetArraySize(layers);
	for (int i = 0; i < layerCount; i++)
	{
		cJSON* layer = cJSON_GetArrayItem(layers, i);
		const char* type =
			cJSON_GetObjectItem(layer, "__type")->valuestring;
		// ============================
// Collision (IntGrid → gCollisionMap)
// ============================
		if (strcmp(type, "IntGrid") == 0)
		{
			cJSON* csv = cJSON_GetObjectItem(layer, "intGridCsv");
			if (!csv) continue;



			int count = cJSON_GetArraySize(csv);
			for (int j = 0; j < count; j++)
			{
				int v = cJSON_GetArrayItem(csv, j)->valueint;

				int x = j % MAP_WIDTH;
				int y = j / MAP_WIDTH;

				if (x >= 0 && x < MAP_WIDTH &&
					y >= 0 && y < MAP_HEIGHT)
				{
					gCollisionMap[y][x] = v;
					// v == 0 → MAP_EMPTY
					// v == 1 → MAP_WALL（LDtk 側と一致させる）
				}
			}
		}
		// ============================
// Entities レイヤー
// ============================
		if (strcmp(type, "Entities") == 0)
		{
			cJSON* entities = cJSON_GetObjectItem(layer, "entityInstances");
			if (!entities) continue;

			int count = cJSON_GetArraySize(entities);

			for (int e = 0; e < count && gEntityCount < MAX_ENTITIES; e++)
			{
				cJSON* ent = cJSON_GetArrayItem(entities, e);

				// 名前 (__identifier)
				const char* id =
					cJSON_GetObjectItem(ent, "__identifier")->valuestring;

				// 位置 (px)
				cJSON* px = cJSON_GetObjectItem(ent, "px");
				int x = cJSON_GetArrayItem(px, 0)->valueint;
				int y = cJSON_GetArrayItem(px, 1)->valueint;

				// サイズ
				int w = cJSON_GetObjectItem(ent, "width")->valueint;
				int h = cJSON_GetObjectItem(ent, "height")->valueint;

				// 保存
				strncpy_s(
					gEntities[gEntityCount].name,
					sizeof(gEntities[gEntityCount].name),
					id,
					_TRUNCATE
				);
				gEntities[gEntityCount].easeFrame = 0.0f;

				gEntities[gEntityCount].x = x;
				gEntities[gEntityCount].y = y;
				gEntities[gEntityCount].w = w;
				gEntities[gEntityCount].h = h;

				EntityType types = ENTITY_UNKNOWN;

				if (strcmp(id, "Entity") == 0)
				{
					types = ENTITY_Entity;
				}
				else if (strcmp(id, "Diagonal_Trampoline_Right") == 0)
				{
					types = ENTITY_Trampoline_R;
				}
				else if (strcmp(id, "Diagonal_Trampoline_Left") == 0)
				{
					types = ENTITY_Trampoline_L;
				}
				else if (strcmp(id, "SwitchR") == 0)
				{
					types = ENTITY_SWITCHR;
				}
				else if (strcmp(id, "Open_sesame") == 0)
				{
					types = ENTITY_OpenSesame;
				}
				else if (strcmp(id, "BreaksWall") == 0)
				{
					types = ENTITY_BREAKSWALL;
				}
				else if (strcmp(id, "Drawn") == 0)
				{
					types = ENTITY_Drawmn;
				}
				else if (strcmp(id, "Warp") == 0)
				{
					types = ENTITY_WARP;
				}

				// ★ 同じ index に全部セット
				strncpy_s(
					gEntities[gEntityCount].name,
					sizeof(gEntities[gEntityCount].name),
					id,
					_TRUNCATE
				);

				gEntities[gEntityCount].x = x;
				gEntities[gEntityCount].y = y;
				gEntities[gEntityCount].w = w;
				gEntities[gEntityCount].h = h;
				gEntities[gEntityCount].types = types;

				gEntities[gEntityCount].startX = x;
				gEntities[gEntityCount].startY = y;
				gEntities[gEntityCount].endX = x;
				gEntities[gEntityCount].endY = y;
				gEntities[gEntityCount].timer = 0.0f;
				gEntities[gEntityCount].warpId = -1; // デフォルト

				cJSON* fields = cJSON_GetObjectItem(ent, "fieldInstances");
				if (fields)
				{
					int fieldCount = cJSON_GetArraySize(fields);
					for (int f = 0; f < fieldCount; f++)
					{
						cJSON* field = cJSON_GetArrayItem(fields, f);

						const char* fname =
							cJSON_GetObjectItem(field, "__identifier")->valuestring;

						cJSON* value = cJSON_GetObjectItem(field, "__value");
						if (!value) continue;

						// --------------------
						// StartPos (XY)
						// --------------------

						// --------------------
						// EndPos (XY)
						// --------------------
						if (strcmp(fname, "EndPos") == 0)
						{
							if (!value || cJSON_IsNull(value)) continue;

							gEntities[gEntityCount].endX =
								cJSON_GetObjectItem(value, "cx")->valueint * TILE_SIZE;
							gEntities[gEntityCount].endY =
								cJSON_GetObjectItem(value, "cy")->valueint * TILE_SIZE;
						}

						// --------------------
						// Timer (Float)
						// --------------------
						else if (strcmp(fname, "Timer") == 0)
						{
							if (!value || cJSON_IsNull(value)) continue;

							gEntities[gEntityCount].timer =
								(float)value->valuedouble;
						}
						else if (strcmp(fname, "warpId") == 0)
						{
							if (!value || cJSON_IsNull(value)) continue;

							gEntities[gEntityCount].warpId = value->valueint;
						}

					}
				}

				// ★ 最後に進める
				gEntityCount++;


			}

			continue; // ★ タイル処理に行かせない
		}

		// ============================
		// 見た目（AutoLayer / Tiles）
		// ============================
		cJSON* tiles = cJSON_GetObjectItem(layer, "gridTiles");
		if (!tiles)
		{
			tiles = cJSON_GetObjectItem(layer, "autoLayerTiles");
		}
		if (!tiles)
		{
			continue;   // ← タイルを持たないレイヤーは無視
		}

		int tileCount = cJSON_GetArraySize(tiles);
		for (int t = 0; t < tileCount; t++)
		{
			cJSON* tile = cJSON_GetArrayItem(tiles, t);

			cJSON* px = cJSON_GetObjectItem(tile, "px");
			int x = cJSON_GetArrayItem(px, 0)->valueint / TILE_SIZE;
			int y = cJSON_GetArrayItem(px, 1)->valueint / TILE_SIZE;

			cJSON* src = cJSON_GetObjectItem(tile, "src");
			int srcX = cJSON_GetArrayItem(src, 0)->valueint;
			int srcY = cJSON_GetArrayItem(src, 1)->valueint;

			int tileIndex =
				(srcY / CHIP_H) * SHEET_COLS +
				(srcX / CHIP_W);

			if (x >= 0 && x < MAP_WIDTH &&
				y >= 0 && y < MAP_HEIGHT)
			{
				gVisualMap[y][x] = tileIndex;
			}
		}

	}

	cJSON_Delete(root);
	return 1;
}

void UpdateEntity()
{
	for (int i = 0; i < gEntityCount; i++)
	{
		Entity& entity = gEntities[i];

		if (entity.types != ENTITY_Drawmn) continue;
		if (entity.timer <= 0.0f) continue;

		// フレーム加算
		entity.easeFrame += 1.0f;

		// 往復周期
		float cycle = entity.timer * 2.0f;

		float t = fmodf(entity.easeFrame, cycle);

		// 折り返し
		if (t > entity.timer)
		{
			t = cycle - t;
		}

		// 0～1
		float norm = t / entity.timer;

		float easeT = easeInOutElastic(norm);

		entity.x = (int)(entity.startX +
			(entity.endX - entity.startX) * easeT);
		entity.y = (int)(entity.startY +
			(entity.endY - entity.startY) * easeT);
	}
}


// ============================
// タイル描画
// ============================
static void DrawTile(int x, int y, int tileIndex)
{
	if (tileIndex < 0) return;

	int srcX = (tileIndex % SHEET_COLS) * CHIP_W;
	int srcY = (tileIndex / SHEET_COLS) * CHIP_H;
	Number++;
	gCollisionMap;
	printf("%d", Number);
	Novice::DrawSpriteRect(
		x, y,
		srcX, srcY,
		CHIP_W, CHIP_H,
		gChipSheetHandle,
		0.05f, 0.0714f,
		0.0f,
		0xFFFFFFFF
	);
}
// ============================
// マップ描画
// ============================
void DrawMapChips(void)
{
	Camera& cam = Camera::Instance();
	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			int tile = gVisualMap[y][x];

			if (tile <= 0) continue;

			int dx = x * TILE_SIZE + (int)cam.x;
			int dy = y * TILE_SIZE + (int)cam.y;
			if (gVisualMap[y][x] >= 0)
			{
				/*Novice::DrawBox(
					dx, dy,
					TILE_SIZE, TILE_SIZE,
					0.0f,
					0xFF0000FF,
					kFillModeWireFrame
				);*/
				DrawTile(dx, dy, tile);
			}
		}
	}

}

void DrawEntities()
{
	Camera& cam = Camera::Instance();

	for (int i = 0; i < gEntityCount; i++)
	{
		int dx = gEntities[i].x + (int)cam.x;
		int dy = gEntities[i].y + (int)cam.y;



		if (strcmp(gEntities[i].name, "Drawn") == 0)
		{
			walkFrameTimer_[0]++;
			if (walkFrameTimer_[0] >= 1) {
				walkFrameTimer_[0] = 0;
				walkFrame_[0]++;
				if (walkFrame_[0] > maxFrame[0]) {
					walkFrame_[0] = 0;
				}
			}

			Novice::DrawSpriteRect(
				dx - gEntities[i].w / 2,
				dy - gEntities[i].h / 2,
				walkFrame_[0] * 128, 0,
				128, 64,
				DrawnTex,
				0.0166666667f, 1.2f, 0.0f,
				0xFFFFFFFF
			);
		}
		else if (strcmp(gEntities[i].name, "Entity") == 0)
		{
			// ★ このトランポリンが踏まれているか
			if (gTrampolineAnim[i].isPlaying)
			{
				int frame = gTrampolineAnim[i].frame;

				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					frame * 200, 0,
					200, 100,
					TranporinJumpTex,
					0.0333333333f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}
			else
			{
				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					0, 0,
					200, 100,
					TranporinTex,
					1.0f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}
		}
		else if (strcmp(gEntities[i].name, "SwitchR") == 0) {
			if (switchState.isActivated) {
				int frameSwict = switchState.frame;
				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					frameSwict * 64, 0,
					64, 128,
					SwitchPusshTex,
					0.0333333333f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}
			else {
				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					0, 0,
					64, 128,
					SwitchTex,
					1.0f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}

		}
		else if (strcmp(gEntities[i].name, "Warp") == 0) {
			walkFrameTimer_[1]++;
			if (walkFrameTimer_[1] >= 1) {
				walkFrameTimer_[1] = 0;
				walkFrame_[1]++;
				if (walkFrame_[1] > maxFrame[1]) {
					walkFrame_[1] = 0;
				}
			}
			Novice::DrawSpriteRect(
				dx - gEntities[i].w / 2,
				dy - gEntities[i].h / 2,
				64 * walkFrame_[1], 0,
				64, 256,
				warpTex,
				0.017f, 1.0f, 0.0f,
				0xFFFFFFFF
			);
		}
		else if (strcmp(gEntities[i].name, "Diagonal_Trampoline_Right") == 0) {
			// ★ このトランポリンが踏まれているか
			if (gTrampolineAnimR[i].isPlaying)
			{
				int frameR = gTrampolineAnimR[i].frame;

  				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					frameR * 64, 0,
					
					64, 64,
					TranporinRjumpTex,
					0.0333333333f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}

			else
			{
				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					0, 0,
					64, 64,
					TranporinRTex,
					1.0f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}
		}
		else if (strcmp(gEntities[i].name, "Diagonal_Trampoline_Left") == 0) {
			// ★ このトランポリンが踏まれているか
			if (gTrampolineAnimL[i].isPlaying)
			{
				int frameL = gTrampolineAnimL[i].frame;

				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					frameL * 64, 0,

					64, 64,
					TranporinLjumpTex,
					0.0333333333f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}

			else
			{
				Novice::DrawSpriteRect(
					dx - gEntities[i].w / 2,
					dy - gEntities[i].h / 2,
					0, 0,
					64, 64,
					TranporinLTex,
					1.0f, 1.0f, 0.0f,
					0xFFFFFFFF
				);
			}
			}

		else
		{
			// 未設定エンティティは赤枠表示
			Novice::DrawBox(
				dx - gEntities[i].w / 2,
				dy - gEntities[i].h / 2,
				gEntities[i].w,
				gEntities[i].h,
				0.0f,
				0xFF0000FF,
				kFillModeSolid
			);
		}


	}
	// ============================
// トランポリンアニメ更新
// ============================
	const int TRAMPOLINE_ANIM_MAX = 30; // Tranporinjump.png のコマ数

	for (int i = 0; i < gEntityCount; i += 5)
	{
		if (!gTrampolineAnim[i].isPlaying) continue;

		gTrampolineAnim[i].frame += 5;

		if (gTrampolineAnim[i].frame >= TRAMPOLINE_ANIM_MAX)
		{
			gTrampolineAnim[i].frame = 0;
			gTrampolineAnim[i].isPlaying = false;
		}
	}

	const int Swict_Max = 30; // Tranporinjump.png のコマ数

	if (switchState.isActivated) {
		switchState.frame += 1;

		if (switchState.frame >= Swict_Max)
		{
			switchState.frame = 0;
			switchState.isActivated = false;
		}
	}

	for (int i = 0; i < gEntityCount; i += 5)
	{
		if (!gTrampolineAnimR[i].isPlaying) continue;
		gTrampolineAnimR[i].frame += 5;
		if (gTrampolineAnimR[i].frame >= 30)
		{
			gTrampolineAnimR[i].frame = 0;
			gTrampolineAnimR[i].isPlaying = false;
		}
	}

	for (int i = 0; i < gEntityCount; i += 5)
	{
		if (!gTrampolineAnimL[i].isPlaying) continue;
		gTrampolineAnimL[i].frame += 5;
		if (gTrampolineAnimL[i].frame >= 30)
		{
			gTrampolineAnimL[i].frame = 0;
			gTrampolineAnimL[i].isPlaying = false;
		}
	}



}