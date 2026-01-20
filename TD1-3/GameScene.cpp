#include "GameScene.h"
#include "Player.h"
#include "MapCollision.h"
#include "camera.h"
#include "Map.h" 
#include <Novice.h>

int kWindowWidth = 1280;
int kWindowHeight = 720;

// 画像ハンドル（シーン内で使用）
static int gChipSheetHandle = -1;

// SceneManager のデストラクタ
SceneManager::~SceneManager() 
{
    delete player_;
}

// SceneManager のコンストラクタ
enum MapNumber
{
    air = -1,
    sand,
    drawn,
};

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
            int id = gMap[y][x]; // Map.h/cpp の gMap を参照
            if (id == -1) continue;

            int chipIndex = id;
            int srcX = (chipIndex % SHEET_COLS) * CHIP_W;
            int srcY = (chipIndex / SHEET_COLS) * CHIP_H;

            // カメラ座標を加算
            int dstX = x * TILE_SIZE + (int)cam.x;
            int dstY = y * TILE_SIZE + (int)cam.y;

            // 画面外なら描画しない
            if (dstX < -TILE_SIZE || dstX > kWindowWidth || dstY < -TILE_SIZE || dstY > kWindowHeight) {
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
                Novice::DrawBox(dstX, dstY, TILE_SIZE, TILE_SIZE, 0.0f, BLACK, kFillModeSolid);
                break;
            case MAP_WARPOUT:
                Novice::DrawBox(dstX, dstY, TILE_SIZE, TILE_SIZE, 0.0f, BLACK, kFillModeSolid);
                break;
            default:
                // その他（トゲ、ワープなど）は赤く表示
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

    int mx, my;
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

    switch (currentScene_) 
    {

    case SceneType::TITLE:
        player_->Initialize();
        itializeMap();

        // 次のシーンへ
        if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
        {
            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::STAGESELECT);
        }

        // ショートカット
        if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4)) 
        {
            LoadMapCSV("./Map/Map4.csv", gMap);
            previousScene_ = SceneType::TITLE;
            StartFade(SceneType::PLAY);
        }
        break;

    case SceneType::STAGESELECT:
        // 遷移時に1回だけロード
        if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10))
        {
            LoadMapCSV("./Map/Map4.csv", gMap);
            previousScene_ = SceneType::STAGESELECT;
            StartFade(SceneType::PLAY);
        }
        break;

    case SceneType::PLAY:
        HitStop::Instance().Update();
        player_->Update();

        // デバッグ用シーン遷移
        if (keys[DIK_2] && !preKeys[DIK_2]) StartFade(SceneType::GAMEOVER);
        if (keys[DIK_1] && !preKeys[DIK_1]) StartFade(SceneType::CLEAR);

        // PAUSE 画面へ
        if ((keys[DIK_P] && !preKeys[DIK_P]) || Novice::IsTriggerButton(0, kPadButton4)) 
        {
            currentScene_ = SceneType::PAUSE;
        }
        break;

    case SceneType::CLEAR:
        if (Novice::IsPressMouse(0) || Novice::IsTriggerButton(0, kPadButton10)) 
        {
            StartFade(SceneType::TITLE);
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

        // マウス判定
        if (Novice::IsTriggerMouse(0)) 
        {
            if (pauseButtons_[0].IsHovered(mx, my)) currentScene_ = SceneType::PLAY;
            if (pauseButtons_[1].IsHovered(mx, my)) StartFade(SceneType::TITLE);
        }

        // 戻る
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

    case SceneType::PLAY:
        DrawMapChips();
        player_->Draw();
        break;

    case SceneType::CLEAR:
        Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameClearImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
        break;

    case SceneType::GAMEOVER:
        Novice::DrawSpriteRect(0, 0, 0, 0, 1280, 720, gameOverImage_, 1.0f, 1.0f, 0.0f, 0xFFFFFFFF);
        break;

    case SceneType::PAUSE:
        // 背景としてゲーム画面を描画
        DrawMapChips();
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

            float spriteW = 900.0f;
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

            Novice::DrawSpriteRect(button.x, button.y, srcX, 0, UIframeW, UIframeH, pauseUI, UIframeW / spriteW, 1.0f, 0.0f, color);
        }
        break;
    }

    if (fadeAlpha_ > 0) 
    {
        unsigned int color = 0x00000000 | fadeAlpha_;
        Novice::DrawBox(0, 0, kWindowWidth, kWindowHeight, 0.0f, color, kFillModeSolid);
    }
}