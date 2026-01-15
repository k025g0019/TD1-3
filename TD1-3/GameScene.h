#pragma once
#include <Novice.h>
#include "Map.h"

// 必要なクラスの前方宣言
// GameScene.cpp で #include されているヘッダーに対応
class Player;

enum class SceneType {
    TITLE,
    STAGESELECT,
    PLAY,
    CLEAR,
    GAMEOVER,
    PAUSE
};

class SceneManager {
public:

    // コンストラクタ / デストラクタ
    SceneManager();
    ~SceneManager();

    // 更新 / 描画
    void Update(char* keys, char* preKeys);
    void Draw();
    
private:
    // ------------------------------------------------------------
    // ▼ 型定義
    // ------------------------------------------------------------
    struct Button {
        int x;
        int y;
        int width;
        int height;
        unsigned int normalColor;
        unsigned int hoverColor;

        /**
         * @brief マウスカーソルがボタン領域内にあるか判定
         *
         * @param mx マウスX座標
         * @param my マウスY座標
         * @return true マウスがホバーしている
         * @return false マウスがホバーしていない
         */
        bool IsHovered(int mx, int my) const {
            return (mx >= x && mx <= x + width &&
                my >= y && my <= y + height);
        }
    };


    // ------------------------------------------------------------
    // ▼ メンバー変数
    // ------------------------------------------------------------

    // --- シーン管理 ---
    SceneType currentScene_ = SceneType::TITLE;
    SceneType previousScene_ = SceneType::TITLE; // 初期値として TITLE を設定
    SceneType nextScene_ = SceneType::TITLE;

    // --- フェード管理 ---
    bool isFading_ = false;
    bool fadeOut_ = true;     // true: フェードアウト（だんだん黒く）
    int fadeAlpha_ = 0;    // 0〜255 のアルファ値
    const int kFadeSpeed_ = 5; // アニメ速度

    // --- UI ボタン ---
    static const int kPauseButtonCount_ = 3;
    Button pauseButtons_[kPauseButtonCount_];
    int pauseUI;

    //========================================================================================================
    // GAMEOVER animation
    int gameOverImage_;        // 画像ハンドル
    int gameClearImage_;
    int animFrame_;
    float animTimer_;
    int TITLEImage = Novice::LoadTexture("./Resource/Image/TITLE.bmp");
    //========================================================================================================
    
    // --- ゲームオブジェクト ---

    Player* player_ = nullptr;
    
    int pauseCursor_ = 0;
    int bgmChannel = -1;
    

    // ------------------------------------------------------------
    // ▼ プライベート関数
    // ------------------------------------------------------------

    /**
     * @brief フェード処理の開始
     *
     * @param next 次のシーン
     */
    void StartFade(SceneType next);


};