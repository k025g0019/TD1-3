#include "GameScene.h"
#include "Player.h"
#include "MapCollision.h"
#include "camera.h"
#include "Map.h" 
#include <Novice.h>
#include <stdio.h>
#include <math.h>

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

// クリア情報を保存する配列
static bool gStageClearFlags[kMaxStages] = { false };

// SceneManager のデストラクタ
SceneManager::~SceneManager()
{
	delete player_;
}

// SceneManager のコンストラクタ
SceneManager::SceneManager()
{

	// 初期シーン設定
	currentScene_ = SceneType::TITLE;
	nextScene_ = currentScene_;
	previousScene_ = currentScene_;

	// フェード初期化
	isFading_ = false;
	fadeOut_ = true;
	fadeAlpha_ = 0;

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
	TITLEImage = Novice::LoadTexture("./Resource/Image/TITLE.bmp");
	pauseUI = Novice::LoadTexture("./Resource/Image/pauseUI.png");
	gameOverImage_ = Novice::LoadTexture("./Resource/Image/gameOVER.jpg");
	gameClearImage_ = Novice::LoadTexture("./Resource/Image/GAMECLEAR.bmp");

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
			int id = gMap[y][x];
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
void SceneManager::Update(char* keys, char* preKeys) {
  
	int mx, my;
	Novice::GetMousePosition(&mx, &my);

	// --- 共通 NEXT ボタン処理 ---
	// [this] を明示的にキャプチャ
	auto HandleNextButton = [this, mx, my](SceneType from) {

		if (!Novice::IsTriggerMouse(0)) return;

		// TITLE → HELP
		if (from == SceneType::TITLE) {
			previousScene_ = SceneType::TITLE;
			StartFade(SceneType::STAGESELECT);
			return;
		}


		};

	//===========================
	// ▼ フェード処理
	//===========================
	if (isFading_) {

		if (fadeOut_) {
			// フェードアウト（だんだん黒く）
			fadeAlpha_ += kFadeSpeed_;
			if (fadeAlpha_ >= 255) {
				fadeAlpha_ = 255;

				// シーン切り替え
				currentScene_ = nextScene_;

				// フェードイン開始
				fadeOut_ = false;
			}
		}
		else {
			// フェードイン（だんだん明るく）
			fadeAlpha_ -= kFadeSpeed_;
			if (fadeAlpha_ <= 0) {
				fadeAlpha_ = 0;
				isFading_ = false; // フェード終了
			}
		}
		return; // フェード中は元の処理を止める
	}


	//===========================
	// ▼ シーン別更新処理
	//===========================
    switch (currentScene_) {

    case SceneType::TITLE:
        //最初期化


        player_->Initialize();
        InitializeMap();

        //マウスの左クリックで次のシーンへ
        if (Novice::IsPressMouse(0) ||
            Novice::IsTriggerButton(0, kPadButton10))
        {
            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::STAGESELECT);
        }

        //タイトルから一気にゲームシーンへ
        if (keys[DIK_P] && !preKeys[DIK_P] ||
            Novice::IsTriggerButton(0, kPadButton4)
            ) {

            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::STAGESELECT);
        }

        break;

    case SceneType::STAGESELECT:
        LoadMapLDtk("./Map/Map1.ldtk");
        if (Novice::IsPressMouse(0) ||
            Novice::IsTriggerButton(0, kPadButton10))
        {
            previousScene_ = SceneType::STAGESELECT;
            StartFade(SceneType::PLAY);
        }
        break;
    case SceneType::PLAY:

        HitStop::Instance().Update();



        player_->Update();

        /*if (player_->CheckTileCollisions(gMap)) {
            StartFade(SceneType::GAMEOVER);
        }*/


        // オブジェクトの振動・カメラの更新



        // --- シーン遷移の条件例（キー操作） ---
        if (keys[DIK_2] && !preKeys[DIK_2]) {
            StartFade(SceneType::GAMEOVER);
        }
        if (keys[DIK_1] && !preKeys[DIK_1]) {
            StartFade(SceneType::CLEAR);
        }

        // --- シーン遷移の条件例---

        //ゲームオーバー

        //クリア



        // --- PAUSE 画面への移行 ---
        if (keys[DIK_P] && !preKeys[DIK_P] || Novice::IsTriggerButton(0, kPadButton4)) {
            // フェードを挟まずに即時切り替え
            currentScene_ = SceneType::PAUSE;
        }
        break;

    case SceneType::CLEAR:
        if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10)) {
            StartFade(SceneType::TITLE);
            animTimer_ = 0;
            animFrame_ = 0;
        }

        animTimer_++;
        if (animTimer_ >= 15) {   // 切り替え速度
            animTimer_ = 0;
            animFrame_++;
            if (animFrame_ >= 4) {
                animFrame_ = 0;
            }
        }


        break;

    case SceneType::GAMEOVER:

        animTimer_++;
        if (animTimer_ >= 10) {   // 切り替え速度
            animTimer_ = 0;
            animFrame_++;
            if (animFrame_ >= 10) {
                animFrame_ = 9;
            }
        }

        // 画面クリックでタイトルへ戻る
        if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10)) {
            StartFade(SceneType::TITLE);
            animTimer_ = 0;
            animFrame_ = 0;

        }
        break;

    case SceneType::PAUSE:

        if (!Novice::IsTriggerMouse(0)) return;

        if (Novice::IsTriggerButton(0, kPadButton0)) pauseCursor_--;
        if (Novice::IsTriggerButton(0, kPadButton1)) pauseCursor_++;

        pauseCursor_ = (pauseCursor_ + 3) % 3;

        if (Novice::IsTriggerButton(0, kPadButton10)) {
            if (pauseCursor_ == 0) currentScene_ = SceneType::PLAY;
            if (pauseCursor_ == 1) StartFade(SceneType::TITLE);
            if (pauseCursor_ == 2) {
                previousScene_ = SceneType::PAUSE;
                StartFade(SceneType::TITLE);
            }
        }


        { // スコープを明示
            // --- ボタンクリック判定 ---
            if (Novice::IsTriggerMouse(0)) {
                if (pauseButtons_[0].IsHovered(mx, my)) {
                    currentScene_ = SceneType::PLAY;  // Resume: フェードなしで即時再開
                }
                if (pauseButtons_[1].IsHovered(mx, my)) {
                    StartFade(SceneType::TITLE);      // Retry: TITLEへ
                }

            }

            Novice::GetMousePosition(&mx, &my);

            // フェード処理
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
        }
    }
    switch (currentScene_) 
    {
    case SceneType::TITLE:
        //最初期化

	// ★マウス移動判定
	static int preMx = 0;
	static int preMy = 0;
	bool isMouseMoved = false;

	if (mx != preMx || my != preMy)
	{
		isMouseMoved = true;
	}
	preMx = mx;
	preMy = my;

        //マウスの左クリックで次のシーンへ
        if (Novice::IsPressMouse(0) ||
            Novice::IsTriggerButton(0, kPadButton10))
        {
            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::STAGESELECT);
        }
        // ショートカット
        if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4)) 
        {
            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::STAGESELECT);
        }

		// マウスの左クリックで次のシーンへ
		if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
		{
			previousScene_ = SceneType::TITLE;
			StartFade(SceneType::STAGESELECT);
		}

		// ショートカット
		if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4))
		{
			LoadMapLDtk("./Map/Map4.csv");
			previousScene_ = SceneType::TITLE;
			StartFade(SceneType::PLAY);
		}
		break;

	case SceneType::STAGESELECT:
	{
		// ----------------------------------------------------------
		// 入力変数
		// ----------------------------------------------------------
		bool moveLeft = false;
		bool moveRight = false;
		bool moveUp = false;
		bool moveDown = false;
		bool isKeyOperated = false; // キーやホイール操作があったか

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

		// 値をそのまま使う
		if (wheel > 0)
		{
			moveUp = true;   // 奥に回したら戻る（上）
		}
		else if (wheel < 0)
		{
			moveDown = true; // 手前に回したら進む（下）
		}

		// キーやホイール操作があったかチェック
		if (moveLeft || moveRight || moveUp || moveDown)
		{
			isKeyOperated = true;
		}

		// ----------------------------------------------------------
		// 移動処理実行
		// ----------------------------------------------------------
		if (moveLeft)  currentStageNo_--;
		if (moveRight) currentStageNo_++;
		if (moveUp)    currentStageNo_ -= kCols;
		if (moveDown)  currentStageNo_ += kCols;

		// 範囲制限・ループ処理
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
		// マウスホバー処理
		// ----------------------------------------------------------
		if (!isKeyOperated && isMouseMoved)
		{
			// 現在のスクロール位置を計算
			int currentRow = currentStageNo_ / kCols;
			static int topRow = 0;
			if (currentRow < topRow) topRow = currentRow;
			if (currentRow >= topRow + kRows) topRow = currentRow - (kRows - 1);

			// 当たり判定
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
		// 決定処理
		// ----------------------------------------------------------
		if ((keys[DIK_SPACE] && !preKeys[DIK_SPACE]) ||
			(keys[DIK_RETURN] && !preKeys[DIK_RETURN]) ||
			Novice::IsTriggerButton(0, kPadButton10) ||
			Novice::IsTriggerMouse(0)) // 左クリック
		{
			char filePath[64];
			sprintf_s(filePath, "./Map/Map%d.LDtk", currentStageNo_ + 1);

			LoadMapLDtk(filePath);

			player_->Initialize();
			previousScene_ = SceneType::STAGESELECT;
			StartFade(SceneType::PLAY);
		}
	}
	break;

	case SceneType::PLAY:
		HitStop::Instance().Update();
		player_->Update();

		// デバッグ: 2でゲームオーバー
		if (keys[DIK_2] && !preKeys[DIK_2]) StartFade(SceneType::GAMEOVER);

		// ★デバッグ: 1でクリア
		if (keys[DIK_1] && !preKeys[DIK_1]) {
			// 現在のステージをクリア済みにする
			if (currentStageNo_ >= 0 && currentStageNo_ < kMaxStages)
			{
				gStageClearFlags[currentStageNo_] = true;
			}
			StartFade(SceneType::CLEAR);
		}

		// PAUSE
		if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4))
		{
			currentScene_ = SceneType::PAUSE;
		}
		break;

	case SceneType::CLEAR:
		if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
		{
			StartFade(SceneType::STAGESELECT);
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
			StartFade(SceneType::TITLE);
			animTimer_ = 0; animFrame_ = 0;
		}
		break;

	case SceneType::PAUSE:
		if (Novice::IsTriggerButton(0, kPadButton0)) pauseCursor_--;
		if (Novice::IsTriggerButton(0, kPadButton1)) pauseCursor_++;

		pauseCursor_ = (pauseCursor_ + 3) % 3;

		if (Novice::IsTriggerButton(0, kPadButton10))
		{
			if (pauseCursor_ == 0) currentScene_ = SceneType::PLAY;
			if (pauseCursor_ == 1) StartFade(SceneType::TITLE);
			if (pauseCursor_ == 2)
			{
				previousScene_ = SceneType::PAUSE;
				StartFade(SceneType::TITLE);
			}
		}

		if (Novice::IsTriggerMouse(0))
		{
			if (pauseButtons_[0].IsHovered(mx, my)) currentScene_ = SceneType::PLAY;
			if (pauseButtons_[1].IsHovered(mx, my)) StartFade(SceneType::TITLE);
		}

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
		Novice::DrawSprite(0, 0, TITLEImage, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

		// 
		// ステージセレクトの描画
	case SceneType::STAGESELECT:
	{
		Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, 0x222244FF, kFillModeSolid);
		Novice::ScreenPrintf(50, 30, "SELECT STAGE");
		Novice::ScreenPrintf(50, 50, "WASD / Arrow / Wheel / Mouse : Select");
		Novice::ScreenPrintf(50, 70, "SPACE / ENTER / PAD-A / Click : Decide");

		// スクロール位置の再計算（表示用）
		int currentRow = currentStageNo_ / kCols;
		static int topRow = 0;
		if (currentRow < topRow) topRow = currentRow;
		if (currentRow >= topRow + kRows) topRow = currentRow - (kRows - 1);

		// グリッド描画
		for (int r = 0; r < kRows; r++)
		{
			for (int c = 0; c < kCols; c++)
			{
				int stageIdx = (topRow + r) * kCols + c;
				if (stageIdx >= kMaxStages) continue;

				int x = kStartX + c * (kTileW + kGapX);
				int y = kStartY + r * (kTileH + kGapY);

				// --- 色の決定 ---
				unsigned int color = 0x444466FF; // 通常色（少し暗い青）

				// クリア済みなら色を変える (シアン)
				bool isCleared = gStageClearFlags[stageIdx];
				if (isCleared)
				{
					color = 0x00AAAAFF;
				}

				// 選択中ならさらに目立つ色に (オレンジ)
				if (stageIdx == currentStageNo_)
				{
					color = 0xFFAA00FF;
					// 選択枠を描画
					Novice::DrawBox(x - 5, y - 5, kTileW + 10, kTileH + 10, 0.0f, 0xFFFFFFAA, kFillModeSolid);
				}

				// ボックス描画
				Novice::DrawBox(x, y, kTileW, kTileH, 0.0f, color, kFillModeSolid);

				// テキスト描画 (白)
				Novice::ScreenPrintf(x + 20, y + 40, "STAGE %d", stageIdx + 1);

				// クリア済みテキストの表示
				if (isCleared) 
				{
					// 右上あたりに「CLEAR!」と表示
					Novice::ScreenPrintf(x + 100, y + 10, "★CLEAR!");
				}
			}
		}

		// スクロールバー
		float progress = (float)currentStageNo_ / (float)(kMaxStages - 1);
		Novice::DrawBox(1200, 150, 10, 490, 0.0f, 0x000000FF, kFillModeSolid);
		Novice::DrawBox(1200, 150 + (int)(470 * progress), 10, 20, 0.0f, 0xFFFFFFFF, kFillModeSolid);
	}
	break;

	case SceneType::PLAY:
		DrawStageMap();
		player_->Draw();
		break;

	case SceneType::CLEAR:
		Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameClearImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

	case SceneType::GAMEOVER:
		Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameOverImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
		break;

	case SceneType::PAUSE:
		// 背景
		DrawStageMap();
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