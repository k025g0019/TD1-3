#include "GameScene.h"
#include "Player.h"
#include "MapCollision.h"
#include "camera.h"
#include "Map.h" 
#include <Novice.h>
#include <stdio.h>
#include <math.h>
enum class StageSelectState {
	SELECT,     // 通常選択
	ENTERING    // ドア突入中
};
// SceneManager.h or cpp のグローバル/メンバ
int selectedDoorX = 0;
int selectedDoorY = 0;

StageSelectState stageSelectState_ = StageSelectState::SELECT;

int kWindowWidth = 1280;
int kWindowHeight = 720;
const int kMaxStages = 100; // ステージ総数

// グリッドUI設定
const int kCols = 5;        // 横に並べる数
const int kRows = 4;        // 縦に表示する数（1画面あたり）
const int kTileW = 180;     // タイルの幅
const int kTileH = 100;     // タイルの高さ
const int kGapX = 40;       // 横の間隔
const int kGapY = 30;       // 縦の間隔
const int kStartX = 110;    // 表示開始X座標
const int kStartY = 150;    // 表示開始Y座標
int Frame[5] = { 0 };
int MaxFrame[5] = { 10, 30,60 };
int FrameTimer[5] = { 0 };
int titlePlayerimage = -1;
int titleFont = -1;
enum class TitlePlayerState {
	IDLE,       // 待機
	CHARGE,     // 溜め
	DASH        // 突進
};

struct TitlePlayer {
	Vector2 pos;

};

enum class DoorType {
	NOSELECT_NOCLEAR = 0,
	SELECT_NOCLEAR,
	NOSELECT_CLEAR,
	SELECT_CLEAR,
	DOOR_TYPE_COUNT
};

struct DoorAnim {
	int frame;
	bool playing;
	int Maxframe;
	int time;
};


int doorTexture[4]; // 4ファイル
DoorAnim doorAnim[kMaxStages];

TitlePlayerState titleState_ = TitlePlayerState::IDLE;
int titleTimer_ = 0;
float titleDashSpeed_ = 0.0f;

TitlePlayer titlePlayer;
// クリア情報を保存する配列
static bool gStageClearFlags[kMaxStages] = { false };

// SceneManager のデストラクタ
SceneManager::~SceneManager()
{
	delete player_;
}

void SceneManager::DrawNumber(int x, int y, int number, float scale)
{
	if (number < 0) number = 0;

	// 桁数計算
	int digits[10];
	int digitCount = 0;

	do {
		digits[digitCount++] = number % 10;
		number /= 10;
	} while (number > 0);

	// 上位桁から描画
	for (int i = digitCount - 1; i >= 0; i--)
	{
		int n = digits[i];
		Novice::DrawSpriteRect(
			x + (digitCount - 1 - i) * int(NUMBER_W * scale),
			y,
			n * NUMBER_W, 0,
			NUMBER_W, NUMBER_H,
			numberTexture,
			0.1f, scale,
			0.0f,
			0xFFFFFFFF
		);
	}
}


void SceneManager::InitTitle()
{
	player_->Initialize();
	for (int i = 0; i < 5; i++) {
		Frame[i] =0;
		FrameTimer[i] =0;
	}

	titleState_ = TitlePlayerState::IDLE;
	titleTimer_ = 0;
	titleDashSpeed_ = 0.0f;
}

// SceneManager のコンストラクタ
SceneManager::SceneManager()
{
	numberTexture = Novice::LoadTexture("./Resource/Image/Number.png");
	actionTex = Novice::LoadTexture("./Resource/Image/Start.png");
	doorTexture[0] = Novice::LoadTexture("./Resource/Image/stageSelectseiteki.png");
	doorTexture[1] = Novice::LoadTexture("./Resource/Image/stageSelect.png");
	doorTexture[2] = Novice::LoadTexture("./Resource/Image/stageSelectclear.png");
	doorTexture[3] = Novice::LoadTexture("./Resource/Image/stageSelectclearDO.png");
	// --- ドアアニメ初期化 ---
	for (int i = 0; i < kMaxStages; i++)
	{
		doorAnim[i].frame = 0;
		doorAnim[i].playing = false;
		doorAnim[i].Maxframe = 30;
		doorAnim[i].time = 0;

	}

	// 初期シーン設定
	currentScene_ = SceneType::TITLE;
	nextScene_ = currentScene_;
	previousScene_ = currentScene_;

	// フェード初期化
	isFading_ = false;
	fadeOut_ = true;
	fadeAlpha_ = 0;
	titlePlayer.pos.x = 640;
	titlePlayer.pos.y = 320;
	// ステージセレクト初期化
	currentStageNo_ = 0;
	
	// ゲーム用オブジェクト生成
	player_ = new Player;

	// ボタン共通設定
	const int kButtonWidth = 300;
	const int kButtonHeight = 60;
	const int kButtonNormalColor = 0x444444ff;
	const unsigned int kButtonHoverColor = 0x00ffffff;

	// PAUSE ボタン初期化
	const int kPauseButtonStartX = kWindowWidth / 2 - (kButtonWidth / 2);
	const int kPauseButtonStartY = kWindowHeight / 2 - 150;
	const int kPauseButtonSpacing = 100;

	for (int i = 0; i < kPauseButtonCount_; i++)
	{
		pauseButtons_[i] =
		{
			 kPauseButtonStartX,
			 kPauseButtonStartY + i * kPauseButtonSpacing,
			 kButtonWidth, kButtonHeight,
			 kButtonNormalColor,   // 通常
			 kButtonHoverColor     // ホバー
		};
	}

	// 画像読み込み
	TITLEImage = Novice::LoadTexture("./Resource/Image/TITLE.png");
	pauseUI = Novice::LoadTexture("./Resource/Image/pauseUI.png");
	gameOverImage_ = Novice::LoadTexture("./Resource/Image/gameOVER.jpg");
	gameClearImage_ = Novice::LoadTexture("./Resource/Image/GAMECLEAR.bmp");
	titlePlayerimage = Novice::LoadTexture("./Resource/Image/player.png");
	titleFont = Novice::LoadTexture("./Resource/Image/titleFont.png");
	// マップチップ画像ロード
	if (gChipSheetHandle == -1)
	{
		gChipSheetHandle = Novice::LoadTexture("./Resource/Image/sand.png");
	}

	animFrame_ = 0;
	animTimer_ = 0.0f;
}

// ------------------------------------------------------------
// マップ描画関数
// ------------------------------------------------------------
void DrawStageMap(void)
{
	Camera& cam = Camera::Instance();
	if (gChipSheetHandle == -1) return;

	for (int y = 0; y < MAP_HEIGHT; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			int id = gVisualMap[y][x];
			if (id == -1) continue;

			int chipIndex = id;
			int srcX = (chipIndex % SHEET_COLS) * CHIP_W;
			int srcY = (chipIndex / SHEET_COLS) * CHIP_H;

			int dstX = x * TILE_SIZE + (int)cam.x;
			int dstY = y * TILE_SIZE + (int)cam.y;

			if (dstX < -TILE_SIZE || dstX > kWindowWidth || dstY < -TILE_SIZE || dstY > kWindowHeight)
			{
				continue;
			}

			switch (id)
			{
			case MAP_WALL:
				Novice::DrawSpriteRect(dstX, dstY, srcX, srcY, CHIP_W, CHIP_H, gChipSheetHandle, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
				break;
			case MAP_GOAL:
				Novice::DrawBox(dstX, dstY, TILE_SIZE, TILE_SIZE, 0.0f, 0x00FF00FF, kFillModeSolid);
				break;
			case MAP_WARPIN:
			case MAP_WARPOUT:
				Novice::DrawBox(dstX, dstY, TILE_SIZE, TILE_SIZE, 0.0f, BLACK, kFillModeSolid);
				break;
			default:
				Novice::DrawBox(dstX, dstY, TILE_SIZE, TILE_SIZE, 0.0f, 0xFF0000FF, kFillModeSolid);
				break;
			}
		}
	}
}

// ------------------------------------------------------------
// ▼ 更新処理
// ------------------------------------------------------------
void SceneManager::Update(char* keys, char* preKeys)
{
	// ------------------------------
// 選択中ドアの論理座標を確定
// ------------------------------
	int currentRow = currentStageNo_ / kCols;
	static int topRow = 0;
	if (currentRow < topRow) topRow = currentRow;
	if (currentRow >= topRow + kRows) topRow = currentRow - (kRows - 1);

	int localIndex = currentStageNo_ - topRow * kCols;
	int doorCol = localIndex % kCols;
	int doorRow = localIndex / kCols;

	selectedDoorX = kStartX + doorCol * (kTileW + kGapX) + (kTileW / 2 - 32);
	selectedDoorY = kStartY + doorRow * (kTileH + kGapY) + 5;


	int mx, my;
	Novice::GetMousePosition(&mx, &my);

	// マウス移動判定（前回と座標が違えば「動いた」とみなす）
	static int preMx = 0;
	static int preMy = 0;
	bool isMouseMoved = false;

	if (mx != preMx || my != preMy)
	{
		isMouseMoved = true;
	}
	preMx = mx;
	preMy = my;

	//===========================
	// ▼ フェード処理
	//===========================
	if (isFading_)
	{
		if (fadeOut_)
		{
			fadeAlpha_ += kFadeSpeed_;
			if (fadeAlpha_ >= 255)
			{
				fadeAlpha_ = 255;
				currentScene_ = nextScene_;
				fadeOut_ = false;
			}
		}
		else
		{
			fadeAlpha_ -= kFadeSpeed_;
			if (fadeAlpha_ <= 0)
			{
				fadeAlpha_ = 0;
				isFading_ = false;
			}
		}
		return; // フェード中は操作を受け付けない
	}

	//===========================
	// ▼ シーン別更新処理
	//===========================
	switch (currentScene_)
	{

	case SceneType::TITLE:

		switch (titleState_) {
			
			// --------------------
			// 待機
			// --------------------
		case TitlePlayerState::IDLE:
			if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] ||
				Novice::IsTriggerButton(0, kPadButton10) ||
				Novice::IsTriggerMouse(0)) {
				titleState_ = TitlePlayerState::CHARGE;
				titleTimer_ = 0;

				// ★チャージ開始エフェクト
			}
			break;

			// --------------------
			// 溜め
			// --------------------
		case TitlePlayerState::CHARGE:
			titleTimer_++;

			// 少し後ろに引く
			titlePlayer.pos.x -= 1.0f;

			if (titleTimer_ >= 30) {
				titleState_ = TitlePlayerState::DASH;
				titleDashSpeed_ = 0.0f;

				// ★破裂エフェクト
			}
			break;

			// --------------------
			// 突進
			// --------------------
		case TitlePlayerState::DASH:
			titleDashSpeed_ += 3.5f;
			titlePlayer.pos.x += titleDashSpeed_;

			if (titlePlayer.pos.x > kWindowWidth + 200) {
				previousScene_ = SceneType::TITLE;
				InitTitle();
				StartFade(SceneType::STAGESELECT);
			}
			break;
		}

		break;



	case SceneType::STAGESELECT:
	{
		if (stageSelectState_ == StageSelectState::ENTERING)
		{
			switch (titleState_)
			{
			case TitlePlayerState::IDLE:
				// 念のための保険（通常ここには来ない）
				titleState_ = TitlePlayerState::CHARGE;
				titleTimer_ = 0;
				titleDashSpeed_ = 0.0f;
				break;

			case TitlePlayerState::CHARGE:
				titleTimer_++;
				titlePlayer.pos.x -= 1.0f;
				if (titleTimer_ >= 30) {
					titleState_ = TitlePlayerState::DASH;
					titleDashSpeed_ = 0.0f;
				}
				break;

			case TitlePlayerState::DASH:
				titleDashSpeed_ += 3.5f;
				titlePlayer.pos.x += titleDashSpeed_;

				if (titlePlayer.pos.x >= selectedDoorX + 40)
				{
					for (int i = 0; i < 5; i++) {
						Frame[i] = 0;
						FrameTimer[i] = 0;
					}
					Camera::Instance().Reset();        // ★最重要
					Camera::Instance().Follow(0, 0);
					InitializeMap();
					LoadMapLDtk("./Map/Map1.ldtk", currentStageNo_);
					player_->Initialize();

					stageSelectState_ = StageSelectState::SELECT;
					titleState_ = TitlePlayerState::IDLE;

					StartFade(SceneType::PLAY);
					HitStop::Instance().Start(START_COUNT_FRAMES);
				}
				break;
			}
			return;
		}






		if (stageSelectState_ == StageSelectState::SELECT)
		{
			// ★ ドアの左に立つだけ（動かさない）
			titlePlayer.pos.x = float(selectedDoorX - 48);   // ドア左
			titlePlayer.pos.y = float(selectedDoorY + 20);   // 足元合わせ
		}

		// ----------------------------------------------------------
		// 入力変数
		// ----------------------------------------------------------
		bool moveLeft = false;
		bool moveRight = false;
		bool moveUp = false;
		bool moveDown = false;
		bool isKeyOperated = false;

		// --- 1. キーボード ---
		if ((keys[DIK_LEFT] && !preKeys[DIK_LEFT]) || (keys[DIK_A] && !preKeys[DIK_A])) moveLeft = true;
		if ((keys[DIK_RIGHT] && !preKeys[DIK_RIGHT]) || (keys[DIK_D] && !preKeys[DIK_D])) moveRight = true;
		if ((keys[DIK_UP] && !preKeys[DIK_UP]) || (keys[DIK_W] && !preKeys[DIK_W])) moveUp = true;
		if ((keys[DIK_DOWN] && !preKeys[DIK_DOWN]) || (keys[DIK_S] && !preKeys[DIK_S])) moveDown = true;

		// --- 2. コントローラー ---
		if (Novice::IsTriggerButton(0, kPadButton2)) moveLeft = true;
		if (Novice::IsTriggerButton(0, kPadButton3)) moveRight = true;
		if (Novice::IsTriggerButton(0, kPadButton0)) moveUp = true;
		if (Novice::IsTriggerButton(0, kPadButton1)) moveDown = true;

		// --- 3. マウスホイール ---
		int wheel = Novice::GetWheel();
		if (wheel > 0) moveUp = true;
		else if (wheel < 0) moveDown = true;

		if (moveLeft || moveRight || moveUp || moveDown) isKeyOperated = true;

		// ----------------------------------------------------------
		// 移動処理
		// ----------------------------------------------------------
		if (moveLeft)  currentStageNo_--;
		if (moveRight) currentStageNo_++;
		if (moveUp)    currentStageNo_ -= kCols;
		if (moveDown)  currentStageNo_ += kCols;

		if (currentStageNo_ < 0)
		{
			currentStageNo_ = kMaxStages - (abs(currentStageNo_) % kMaxStages);
			if (currentStageNo_ == kMaxStages) currentStageNo_ = 0;
		}
		if (currentStageNo_ >= kMaxStages)
		{
			currentStageNo_ = currentStageNo_ % kMaxStages;
		}

		// ----------------------------------------------------------
		// マウスホバー
		// ----------------------------------------------------------
		if (!isKeyOperated && isMouseMoved)
		{

			if (currentRow < topRow) topRow = currentRow;
			if (currentRow >= topRow + kRows) topRow = currentRow - (kRows - 1);

			for (int r = 0; r < kRows; r++)
			{
				for (int c = 0; c < kCols; c++)
				{
					int stageIdx = (topRow + r) * kCols + c;
					if (stageIdx >= kMaxStages) continue;

					int x = kStartX + c * (kTileW + kGapX);
					int y = kStartY + r * (kTileH + kGapY);

					if (mx >= x && mx <= x + kTileW && my >= y && my <= y + kTileH)
					{
						currentStageNo_ = stageIdx;
					}
				}
			}
		}

		// ----------------------------------------------------------
		// ドア：選択変更検出 → アニメ開始
		// ----------------------------------------------------------
		static int prevStage = -1;
		if (currentStageNo_ != prevStage)
		{
			if (prevStage >= 0 && prevStage < kMaxStages)
			{
				doorAnim[prevStage].playing = false;
				doorAnim[prevStage].frame = 0;
			}
			doorAnim[currentStageNo_].playing = true;
			doorAnim[currentStageNo_].frame = 0;
			prevStage = currentStageNo_;
		}

		// ----------------------------------------------------------
		// ドアアニメ更新（選択中のみ）
		// ----------------------------------------------------------
		const int DOOR_ANIM_MAX = 30; // フレーム数
		if (doorAnim[currentStageNo_].playing)
		{
			doorAnim[currentStageNo_].frame++;
			if (doorAnim[currentStageNo_].frame >= DOOR_ANIM_MAX - 1)
			{
				doorAnim[currentStageNo_].frame = DOOR_ANIM_MAX - 1;
				doorAnim[currentStageNo_].playing = false; // 開きっぱ
			}
		}

		// ----------------------------------------------------------
		// フェード中は入力無効
		// ----------------------------------------------------------
		if (isFading_)
		{
			if (fadeOut_)
			{
				fadeAlpha_ += kFadeSpeed_;
				if (fadeAlpha_ >= 255)
				{
					fadeAlpha_ = 255;
					currentScene_ = nextScene_;
					fadeOut_ = false;
				}
			}
			else
			{
				fadeAlpha_ -= kFadeSpeed_;
				if (fadeAlpha_ <= 0)
				{
					fadeAlpha_ = 0;
					isFading_ = false;
				}
			}
			return;
		}

		// ----------------------------------------------------------
		// 決定処理（ドアが開ききった後のみ）
		// ----------------------------------------------------------
		if (
			(
				(keys[DIK_SPACE] && !preKeys[DIK_SPACE]) ||
				(keys[DIK_RETURN] && !preKeys[DIK_RETURN]) ||
				Novice::IsTriggerButton(0, kPadButton10) ||
				Novice::IsTriggerMouse(0)
				))
		{

			stageSelectState_ = StageSelectState::ENTERING;
			titleState_ = TitlePlayerState::CHARGE;
			titleTimer_ = 0;
			titleDashSpeed_ = 0.0f;
			return;
		}


		break;
	}

	case SceneType::PLAY:
		HitStop::Instance().Update();
		// ★カウントダウン中は更新しない
		if (HitStop::Instance().IsActive()) {
			break;
		}

		UpdateEntity();

		player_->Update();


		if (player_->IsDead())
		{
			StartFade(SceneType::GAMEOVER);
			return;
		}
		if (player_->CheckTileCollisions())
		{
			StartFade(SceneType::GAMEOVER);
		}
		if (player_->isGrounded_) {
			if (abs(player_->status.vel.x) <= 20.0f) {
				StartFade(SceneType::GAMEOVER);
				
			}
		}
		if (player_->status.pos.x >= 2560.0f) {
			if (currentStageNo_ >= 0 && currentStageNo_ < kMaxStages)
			{
				gStageClearFlags[currentStageNo_] = true;
			}
			for (int i = 0; i < 5; i++) {
				Frame[i] = 0;
				FrameTimer[i] = 0;
			}
			StartFade(SceneType::CLEAR);
		}

		// デバッグ: 2でゲームオーバー
		if (keys[DIK_2] && !preKeys[DIK_2]) StartFade(SceneType::GAMEOVER);

		// ★デバッグ: 1でクリア
		if (keys[DIK_1] && !preKeys[DIK_1])
		{
			if (currentStageNo_ >= 0 && currentStageNo_ < kMaxStages)
			{
				gStageClearFlags[currentStageNo_] = true;
			}
			StartFade(SceneType::CLEAR);
		}

		// PAUSE画面へ
		if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4))
		{
			currentScene_ = SceneType::PAUSE;
		}
		break;

	case SceneType::CLEAR:
		if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
		{
			StartFade(SceneType::STAGESELECT); // クリア後はセレクトに戻る設定に統一
			animTimer_ = 0; animFrame_ = 0;
		}
		animTimer_++;
		if (animTimer_ >= 15)
		{
			animTimer_ = 0; animFrame_++;
			if (animFrame_ >= 4) animFrame_ = 0;
		}
		break;

	case SceneType::GAMEOVER:
		animTimer_++;
		if (animTimer_ >= 10)
		{
			animTimer_ = 0; animFrame_++;
			if (animFrame_ >= 10) animFrame_ = 9;
		}
		if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
		{
			StartFade(SceneType::STAGESELECT);
			animTimer_ = 0; animFrame_ = 0;
		}
		break;

	case SceneType::PAUSE:
		// カーソル移動
		if (Novice::IsTriggerButton(0, kPadButton0))
		{
			pauseCursor_--;
		}
		if (Novice::IsTriggerButton(0, kPadButton1))
		{
			pauseCursor_++;
		}
		pauseCursor_ = (pauseCursor_ + 3) % 3;

		// 決定ボタン
		if (Novice::IsTriggerButton(0, kPadButton10))
		{
			if (pauseCursor_ == 0)
			{
				currentScene_ = SceneType::PLAY;
			}
			if (pauseCursor_ == 1)
			{
				StartFade(SceneType::STAGESELECT);
			}
			if (pauseCursor_ == 2)
			{
				previousScene_ = SceneType::PAUSE;
				StartFade(SceneType::STAGESELECT);
			}
		}

		// マウス判定
		if (Novice::IsTriggerMouse(0))
		{
			if (pauseButtons_[0].IsHovered(mx, my))
			{
				currentScene_ = SceneType::PLAY;
			}
			if (pauseButtons_[1].IsHovered(mx, my))
			{
				StartFade(SceneType::STAGESELECT);
			}
		}

		// 戻るボタン
		if ((keys[DIK_P] && !preKeys[DIK_P]) ||
			Novice::IsTriggerButton(0, kPadButton4) ||
			Novice::IsTriggerButton(0, kPadButton11))
		{
			currentScene_ = SceneType::PLAY;
		}
		break;
	}
}
// ------------------------------------------------------------
// ▼ フェード開始
// ------------------------------------------------------------
void SceneManager::StartFade(SceneType next)
{
	nextScene_ = next;
	fadeOut_ = true;
	isFading_ = true;
	fadeAlpha_ = 0;
}

// ------------------------------------------------------------
// ▼ 描画処理
// ------------------------------------------------------------
void SceneManager::Draw()
{

	int mx, my;
	Novice::GetMousePosition(&mx, &my);

	switch (currentScene_)
	{

	case SceneType::TITLE:
		FrameTimer[0]++;
		if (FrameTimer[0] >= 3) {
			Frame[0]++;
			FrameTimer[0] = 0;
			if (Frame[0] >= MaxFrame[0]) {
				Frame[0] = 0;
			}
		}
		Novice::DrawSpriteRect(
			0, 0,
			640 * Frame[0], 0, 640, 360
			, TITLEImage, 0.2f, 2.0f, 0.0f, 0xFFFFFFFF);

		FrameTimer[1]++;
		if (FrameTimer[1] >= 1) {
			Frame[1]++;
			FrameTimer[1] = 0;
			if (Frame[1] >= MaxFrame[1]) {
				Frame[1] = 0;
			}
		}
		Novice::DrawSpriteRect(
			int(titlePlayer.pos.x - player_->status.radius), int(titlePlayer.pos.y - player_->status.radius),
			Frame[1] * 500, 0,
			500, 500,
			titlePlayerimage,
			0.00333333333f, 0.1f, 0.0f,
			0xFFFFFFFF
		);

		Novice::DrawSprite(0, 0, titleFont, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

		// 
		// ステージセレクトの描画
	case SceneType::STAGESELECT:
	{
		// ------------------------------
		// 背景アニメ
		// ------------------------------
		FrameTimer[0]++;
		if (FrameTimer[0] >= 3)
		{
			Frame[0]++;
			FrameTimer[0] = 0;
			if (Frame[0] >= MaxFrame[0]) Frame[0] = 0;
		}

		Novice::DrawSpriteRect(
			0, 0,
			640 * Frame[0], 0, 640, 360,
			TITLEImage,
			0.2f, 2.0f, 0.0f,
			0xFFFFFFFF
		);


		// ------------------------------
		// UIテキスト
		// ------------------------------
		Novice::ScreenPrintf(50, 30, "SELECT STAGE");
		Novice::ScreenPrintf(50, 50, "WASD / Arrow / Wheel / Mouse : Select");
		Novice::ScreenPrintf(50, 70, "SPACE / ENTER / PAD-A / Click : Decide");

		// ------------------------------
		// スクロール位置計算
		// ------------------------------
		int currentRow = currentStageNo_ / kCols;
		static int topRow = 0;
		if (currentRow < topRow) topRow = currentRow;
		if (currentRow >= topRow + kRows) topRow = currentRow - (kRows - 1);
		FrameTimer[1]++;
		if (FrameTimer[1] >= 1) {
			Frame[1]++;
			FrameTimer[1] = 0;
			if (Frame[1] >= MaxFrame[1]) {
				Frame[1] = 0;
			}
		}
		// ------------------------------
		// グリッド描画
		// ------------------------------
		for (int r = 0; r < kRows; r++)
		{
			for (int c = 0; c < kCols; c++)
			{
				int stageIdx = (topRow + r) * kCols + c;
				if (stageIdx >= kMaxStages) continue;

				int x = kStartX + c * (kTileW + kGapX);
				int y = kStartY + r * (kTileH + kGapY);

				bool isSelected = (stageIdx == currentStageNo_);
				bool isCleared = gStageClearFlags[stageIdx];

				// --------------------------
				// 背景ボックス色
				// --------------------------
				unsigned int color = 0x444466FF;
				if (isCleared) color = 0x00AAAAFF;

				if (isSelected)
				{
					color = 0xFFAA00FF;
					Novice::DrawBox(
						x - 5, y - 5,
						kTileW + 10, kTileH + 10,
						0.0f,
						0xFFFFFFAA,
						kFillModeSolid
					);

					
				}


				Novice::DrawBox(
					x, y,
					kTileW, kTileH,
					0.0f,
					color,
					kFillModeSolid
				);
				
				if (isSelected) {
					Novice::DrawSpriteRect(
						(int)titlePlayer.pos.x,
						(int)titlePlayer.pos.y,
						Frame[1] * 500, 0,
						500, 500,
						titlePlayerimage,
						0.00333333333f, 0.1f, 0.0f,
						0xFFFFFFFF
					);

	

					
				}
				// ------------------------------
			// スクロールバー
			// ------------------------------
				float progress = (float)currentStageNo_ / (float)(kMaxStages - 1);
				Novice::DrawBox(1200, 150, 10, 490, 0.0f, 0x000000FF, kFillModeSolid);
				Novice::DrawBox(1200, 150 + (int)(470 * progress), 10, 20, 0.0f, 0xFFFFFFFF, kFillModeSolid);
				// --------------------------
				// ステージ番号
				// --------------------------
				// "STAGE" 文字（必要なら画像 or ScreenPrintf）
				Novice::ScreenPrintf(x + 20, y + 15, "STAGE");

				// 数字だけ画像で描画
				DrawNumber(
					x + 20,
					y + 45,
					stageIdx + 1,
					0.8f   // スケール調整
				);


				if (isCleared)
				{
					Novice::ScreenPrintf(x + 100, y + 10, "★CLEAR!");
				}

				// --------------------------
				// ドア種別決定
				// --------------------------
				DoorType doorType;
				if (!isSelected && !isCleared) doorType = DoorType::NOSELECT_NOCLEAR;
				else if (isSelected && !isCleared) doorType = DoorType::SELECT_NOCLEAR;
				else if (!isSelected && isCleared) doorType = DoorType::NOSELECT_CLEAR;
				else                               doorType = DoorType::SELECT_CLEAR;
				// --------------------------
				// ドア描画（アニメ/静止で分岐）
				// --------------------------
				int doorX = x + (kTileW / 2 - 32);
				int doorY = y + 5;
	

				if (isSelected)
				{
					Novice::DrawSpriteRect(
						doorX, doorY,
						doorAnim[stageIdx].frame * 180, 0,
						180, 100,
						doorTexture[(int)doorType],
						0.0333333333f, 1.0f, 0.0f,
						0xFFFFFFFF
					);
				}
				else
				{
					// 未選択：静止（1枚絵）
					Novice::DrawSprite(
						doorX, doorY,
						doorTexture[(int)doorType],
						1.0f, 1.0f, 0.0f,
						0xFFFFFFFF
					);
				}

			}
			
			

		}
		
		
	}
	break;

	case SceneType::PLAY:
		DrawMapChips();
		CloudDraw();
		player_->Draw();
		DrawEntities();
		if (HitStop::Instance().IsActive()) {
			FrameTimer[2]++;
			if (FrameTimer[2]>=3) {
				Frame[2]++;
				FrameTimer[2] = 0;
				if (Frame[2] > MaxFrame[2]) {
					Frame[2] = MaxFrame[2];
				}
			}

			Novice::DrawSpriteRect(0, 255, Frame[2] * 300, 0, 300, 70, actionTex, 0.0975609756f, 3.0f, 0.0f, 0xFFFFFFFF);
		}
		break;

	case SceneType::CLEAR:
		Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameClearImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

	case SceneType::GAMEOVER:
		Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameOverImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

	case SceneType::PAUSE:
		// 背景
		DrawMapChips();
		DrawEntities();
		player_->Draw();
		Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x00000088, kFillModeSolid);

		for (int i = 0; i < kPauseButtonCount_; i++)
		{
			const Button& button = pauseButtons_[i];

			bool mouseHover = button.IsHovered(mx, my);
			bool padHover = false;

			if (Novice::GetNumberOfJoysticks() >= 1)
			{
				if (i == pauseCursor_)
				{
					padHover = true;
				}
			}

			bool hover = false;
			if (mouseHover || padHover)
			{
				hover = true;
			}

			unsigned int color;
			if (hover)
			{
				color = button.hoverColor;
			}
			else
			{
				color = button.normalColor;
			}
			int UIframeW = 300;
			int UIframeH = 60;

			int srcX = 0;
			if (i == 0)
			{
				srcX = 0;
			}
			else if (i == 1)
			{
				srcX = 300;
			}
			else
			{
				srcX = 600;
			}

			Novice::DrawSpriteRect(button.x, button.y, srcX, 0, UIframeW, UIframeH, pauseUI, UIframeW / 900.0f, 1.0f, 0.0f, color);
		}
		break;
	}

	if (fadeAlpha_ > 0)
	{
		unsigned int color = 0x00000000 | fadeAlpha_;
		Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, color, kFillModeSolid);
	}
}