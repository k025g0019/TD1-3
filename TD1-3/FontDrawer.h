#pragma once
#pragma once
#include <cstdint>

// 初期化（画像ロード）
void FontDrawer_Initialize();

// 文字列描画
// x, y : 画面座標（左上）
// scale : 拡大率
// color : ARGB
void DrawBitmapString(
    int x,
    int y,
    const char* text,
    float scale,
    uint32_t color = 0xFFFFFFFF
);
