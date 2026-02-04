// FontDrawer.cpp
#include "FontDrawer.h"
#include "Novice.h"
#include <string.h>
#include <stdint.h>

// ===============================
// フォントテクスチャ
// ===============================
static int gTexHiragana = -1;
static int gTexKatakana = -1;
static int gTexAlphabet = -1;
static int gTexTokusyu = -1;

// ===============================
// 1グリフの基本サイズ（スプライト側の切り抜きサイズ）
// ===============================
static const int GLYPH_W = 64;
static const int GLYPH_H = 64;

struct Glyph {
    const char* ch; // UTF-8 1文字
    int sx;         // 画像内の切り抜き開始X
    int sy;         // 画像内の切り抜き開始Y
};

// ============================================================
// ここを「画像の並び順に完全一致」させて増やしていく
// ============================================================

// ---- ひらがな（例：一部だけ）----
static const Glyph HIRA_GLYPHS[] = {
    {"あ",  GLYPH_W * 0, GLYPH_H * 0},
    {"い",  GLYPH_W * 0, GLYPH_H * 1},
    {"う",  GLYPH_W * 0, GLYPH_H * 2},
    {"え",  GLYPH_W * 0, GLYPH_H * 3},
    {"お",  GLYPH_W * 0, GLYPH_H * 4},

   {"か",  GLYPH_W * 1, GLYPH_H * 0},
    {"き",  GLYPH_W * 1, GLYPH_H * 1},
    {"く",  GLYPH_W * 1, GLYPH_H * 2},
    {"け",  GLYPH_W * 1, GLYPH_H * 3},
    {"こ",  GLYPH_W * 1, GLYPH_H * 4},

    {"さ",  GLYPH_W * 2, GLYPH_H * 0},
    {"し",  GLYPH_W * 2, GLYPH_H * 1},
    {"す",  GLYPH_W * 2, GLYPH_H * 2},
    {"せ",  GLYPH_W * 2, GLYPH_H * 3},
    {"そ",  GLYPH_W * 2, GLYPH_H * 4},

    {"た",  GLYPH_W * 3, GLYPH_H * 0},
    {"ち",  GLYPH_W * 3, GLYPH_H * 1},
    {"つ",  GLYPH_W * 3, GLYPH_H * 2},
    {"て",  GLYPH_W * 3, GLYPH_H * 3},
    {"と",  GLYPH_W * 3, GLYPH_H * 4},

    {"な",  GLYPH_W * 4, GLYPH_H * 0},
    {"に",  GLYPH_W * 4, GLYPH_H * 1},
    {"ぬ",  GLYPH_W * 4, GLYPH_H * 2},
    {"ね",  GLYPH_W * 4, GLYPH_H * 3},
    {"の",  GLYPH_W * 4, GLYPH_H * 4},

    {"は",  GLYPH_W * 5, GLYPH_H * 0},
    {"ひ",  GLYPH_W * 5, GLYPH_H * 1},
    {"ふ",  GLYPH_W * 5, GLYPH_H * 2},
    {"へ",  GLYPH_W * 5, GLYPH_H * 3},
    {"ほ",  GLYPH_W * 5, GLYPH_H * 4},

    {"ま",  GLYPH_W * 6, GLYPH_H * 0},
    {"み",  GLYPH_W * 6, GLYPH_H * 1},
    {"む",  GLYPH_W * 6, GLYPH_H * 2},
    {"め",  GLYPH_W * 6, GLYPH_H * 3},
    {"も",  GLYPH_W * 6, GLYPH_H * 4},

    {"や",  GLYPH_W * 7, GLYPH_H * 0},
    {"ゆ",  GLYPH_W * 7, GLYPH_H * 2},
    {"よ",  GLYPH_W * 7, GLYPH_H * 4},


    {"ら",  GLYPH_W * 8, GLYPH_H * 0},
    {"り",  GLYPH_W * 8, GLYPH_H * 1},
    {"る",  GLYPH_W * 8, GLYPH_H * 2},
    {"れ",  GLYPH_W * 8, GLYPH_H * 3},
    {"ろ",  GLYPH_W * 8, GLYPH_H * 4},

    {"わ",  GLYPH_W * 9, GLYPH_H * 0},
    {"を",  GLYPH_W * 9, GLYPH_H * 4},

    {"ん",  GLYPH_W * 10, GLYPH_H * 0},


    {"ぁ",  GLYPH_W * 0, GLYPH_H * 5},
    {"ぃ",  GLYPH_W * 0, GLYPH_H * 6},
    {"ぅ",  GLYPH_W * 0, GLYPH_H * 7},
    {"ぇ",  GLYPH_W * 0, GLYPH_H * 8},
    {"ぉ",  GLYPH_W * 0, GLYPH_H * 9},

    {"ヵ",  GLYPH_W * 1, GLYPH_H * 5},

    {"ヶ",  GLYPH_W * 1, GLYPH_H * 8},



    {"っ",  GLYPH_W * 3, GLYPH_H * 2},


    {"ゃ",  GLYPH_W * 7, GLYPH_H * 5},
    {"ゅ",  GLYPH_W * 7, GLYPH_H * 7},
    {"ょ",  GLYPH_W * 7, GLYPH_H * 9},

    {"ゎ",  GLYPH_W * 9, GLYPH_H * 0},




   {"が",  GLYPH_W * 1, GLYPH_H * 10},
    {"ぎ",  GLYPH_W * 1, GLYPH_H * 11},
    {"ぐ",  GLYPH_W * 1, GLYPH_H * 12},
    {"げ",  GLYPH_W * 1, GLYPH_H * 13},
    {"ご",  GLYPH_W * 1, GLYPH_H * 14},

    {"ざ",  GLYPH_W * 2, GLYPH_H * 10},
    {"じ",  GLYPH_W * 2, GLYPH_H * 11},
    {"ず",  GLYPH_W * 2, GLYPH_H * 12},
    {"ぜ",  GLYPH_W * 2, GLYPH_H * 13},
    {"ぞ",  GLYPH_W * 2, GLYPH_H * 14},

    {"だ",  GLYPH_W * 3, GLYPH_H * 10},
    {"ぢ",  GLYPH_W * 3, GLYPH_H * 11},
    {"づ",  GLYPH_W * 3, GLYPH_H * 12},
    {"で",  GLYPH_W * 3, GLYPH_H * 13},
    {"ど",  GLYPH_W * 3, GLYPH_H * 14},

    { "ば",  GLYPH_W * 5, GLYPH_H * 10 },
    { "び",  GLYPH_W * 5, GLYPH_H * 11 },
    { "ぶ",  GLYPH_W * 5, GLYPH_H * 12 },
    { "べ",  GLYPH_W * 5, GLYPH_H * 13 },
    { "ぼ",  GLYPH_W * 5, GLYPH_H * 14 },

    { "ぱ",  GLYPH_W * 6, GLYPH_H * 10 },
    { "ぴ",  GLYPH_W * 6, GLYPH_H * 11 },
    { "ぷ",  GLYPH_W * 6, GLYPH_H * 12 },
    { "ぺ",  GLYPH_W * 6, GLYPH_H * 13 },
    { "ぽ",  GLYPH_W * 6, GLYPH_H * 14 },

    // 以降、必要なだけ追加
};
#define HIRA_GLYPH_COUNT (int)(sizeof(HIRA_GLYPHS) / sizeof(HIRA_GLYPHS[0]))

// ---- カタカナ（例：一部だけ）----
static const Glyph KATA_GLYPHS[] = {
       {"ア",  GLYPH_W * 0, GLYPH_H * 0},
    {"イ",  GLYPH_W * 0, GLYPH_H * 1},
    {"ウ",  GLYPH_W * 0, GLYPH_H * 2},
    {"エ",  GLYPH_W * 0, GLYPH_H * 3},
    {"オ",  GLYPH_W * 0, GLYPH_H * 4},

   {"カ",  GLYPH_W * 1, GLYPH_H * 0},
    {"キ",  GLYPH_W * 1, GLYPH_H * 1},
    {"ク",  GLYPH_W * 1, GLYPH_H * 2},
    {"ケ",  GLYPH_W * 1, GLYPH_H * 3},
    {"コ",  GLYPH_W * 1, GLYPH_H * 4},

    {"サ",  GLYPH_W * 2, GLYPH_H * 0},
    {"シ",  GLYPH_W * 2, GLYPH_H * 1},
    {"ス",  GLYPH_W * 2, GLYPH_H * 2},
    {"セ",  GLYPH_W * 2, GLYPH_H * 3},
    {"ソ",  GLYPH_W * 2, GLYPH_H * 4},

    {"タ",  GLYPH_W * 3, GLYPH_H * 0},
    {"チ",  GLYPH_W * 3, GLYPH_H * 1},
    {"ツ",  GLYPH_W * 3, GLYPH_H * 2},
    {"テ",  GLYPH_W * 3, GLYPH_H * 3},
    {"ト",  GLYPH_W * 3, GLYPH_H * 4},

    {"ナ",  GLYPH_W * 4, GLYPH_H * 0},
    {"ニ",  GLYPH_W * 4, GLYPH_H * 1},
    {"ヌ",  GLYPH_W * 4, GLYPH_H * 2},
    {"ネ",  GLYPH_W * 4, GLYPH_H * 3},
    {"ノ",  GLYPH_W * 4, GLYPH_H * 4},

    {"ハ",  GLYPH_W * 5, GLYPH_H * 0},
    {"ヒ",  GLYPH_W * 5, GLYPH_H * 1},
    {"フ",  GLYPH_W * 5, GLYPH_H * 2},
    {"ヘ",  GLYPH_W * 5, GLYPH_H * 3},
    {"ホ",  GLYPH_W * 5, GLYPH_H * 4},

    {"マ",  GLYPH_W * 6, GLYPH_H * 0},
    {"ミ",  GLYPH_W * 6, GLYPH_H * 1},
    {"ム",  GLYPH_W * 6, GLYPH_H * 2},
    {"メ",  GLYPH_W * 6, GLYPH_H * 3},
    {"モ",  GLYPH_W * 6, GLYPH_H * 4},

    {"ヤ",  GLYPH_W * 7, GLYPH_H * 0},
    {"ユ",  GLYPH_W * 7, GLYPH_H * 2},
    {"ヨ",  GLYPH_W * 7, GLYPH_H * 4},


    {"ラ",  GLYPH_W * 8, GLYPH_H * 0},
    {"リ",  GLYPH_W * 8, GLYPH_H * 1},
    {"ル",  GLYPH_W * 8, GLYPH_H * 2},
    {"レ",  GLYPH_W * 8, GLYPH_H * 3},
    {"ロ",  GLYPH_W * 8, GLYPH_H * 4},

    {"ワ",  GLYPH_W * 9, GLYPH_H * 0},
    {"ヲ",  GLYPH_W * 9, GLYPH_H * 4},

    {"ン",  GLYPH_W * 10, GLYPH_H * 0},

    {"ァ",  GLYPH_W * 0, GLYPH_H * 5},
    {"ィ",  GLYPH_W * 0, GLYPH_H * 6},
    {"ゥ",  GLYPH_W * 0, GLYPH_H * 7},
    {"ェ",  GLYPH_W * 0, GLYPH_H * 8},
    {"ォ",  GLYPH_W * 0, GLYPH_H * 9},

    {"ヵ",  GLYPH_W * 1, GLYPH_H * 5},

    {"ヶ",  GLYPH_W * 1, GLYPH_H * 8},



    {"ッ",  GLYPH_W * 3, GLYPH_H * 2},


    {"ャ",  GLYPH_W * 7, GLYPH_H * 5},
    {"ュ",  GLYPH_W * 7, GLYPH_H * 7},
    {"ョ",  GLYPH_W * 7, GLYPH_H * 9},

    {"ヮ",  GLYPH_W * 9, GLYPH_H * 0},




   {"ガ",  GLYPH_W * 1, GLYPH_H * 10},
    {"ギ",  GLYPH_W * 1, GLYPH_H * 11},
    {"グ",  GLYPH_W * 1, GLYPH_H * 12},
    {"ゲ",  GLYPH_W * 1, GLYPH_H * 13},
    {"ゴ",  GLYPH_W * 1, GLYPH_H * 14},

    {"ザ",  GLYPH_W * 2, GLYPH_H * 10},
    {"ジ",  GLYPH_W * 2, GLYPH_H * 11},
    {"ズ",  GLYPH_W * 2, GLYPH_H * 12},
    {"ゼ",  GLYPH_W * 2, GLYPH_H * 13},
    {"ゾ",  GLYPH_W * 2, GLYPH_H * 14},

    {"ダ",  GLYPH_W * 3, GLYPH_H * 10},
    {"ヂ",  GLYPH_W * 3, GLYPH_H * 11},
    {"ヅ",  GLYPH_W * 3, GLYPH_H * 12},
    {"デ",  GLYPH_W * 3, GLYPH_H * 13},
    {"ド",  GLYPH_W * 3, GLYPH_H * 14},

    { "バ",  GLYPH_W * 5, GLYPH_H * 10 },
    { "ビ",  GLYPH_W * 5, GLYPH_H * 11 },
    { "ブ",  GLYPH_W * 5, GLYPH_H * 12 },
    { "ベ",  GLYPH_W * 5, GLYPH_H * 13 },
    { "ボ",  GLYPH_W * 5, GLYPH_H * 14 },

    { "パ",  GLYPH_W * 6, GLYPH_H * 10 },
    { "ピ",  GLYPH_W * 6, GLYPH_H * 11 },
    { "プ",  GLYPH_W * 6, GLYPH_H * 12 },
    { "ペ",  GLYPH_W * 6, GLYPH_H * 13 },
    { "ポ",  GLYPH_W * 6, GLYPH_H * 14 },


    // 以降、必要なだけ追加
};
#define KATA_GLYPH_COUNT (int)(sizeof(KATA_GLYPHS) / sizeof(KATA_GLYPHS[0]))

// ---- 英数字（例：一部だけ）----
// ※ arufabeto.png の並びに合わせて座標を書いて増やす
static const Glyph ALPHA_GLYPHS[] = {
    {"A", GLYPH_W * 0, GLYPH_H * 0},
    {"B", GLYPH_W * 1, GLYPH_H * 0},
    {"C", GLYPH_W * 2, GLYPH_H * 0},
    {"D", GLYPH_W * 3, GLYPH_H * 0},
    {"E", GLYPH_W * 4, GLYPH_H * 0},
    {"F", GLYPH_W * 5, GLYPH_H * 0},
    {"G", GLYPH_W * 6, GLYPH_H * 0},
    {"H", GLYPH_W * 7, GLYPH_H * 0},
    {"I", GLYPH_W * 8, GLYPH_H * 0},
    {"J", GLYPH_W * 9, GLYPH_H * 0},
    {"K", GLYPH_W * 10, GLYPH_H * 0},

    {"L", GLYPH_W * 0, GLYPH_H * 1},
    {"M", GLYPH_W * 1, GLYPH_H * 1},
    {"N", GLYPH_W * 2, GLYPH_H * 1},
    {"O", GLYPH_W * 3, GLYPH_H * 1},
    {"P", GLYPH_W * 4, GLYPH_H * 1},
    {"Q", GLYPH_W * 5, GLYPH_H * 1},
    {"R", GLYPH_W * 6, GLYPH_H * 1},
    {"S", GLYPH_W * 7, GLYPH_H * 1},
    {"T", GLYPH_W * 8, GLYPH_H * 1},
    {"U", GLYPH_W * 9, GLYPH_H * 1},
    {"V", GLYPH_W * 10, GLYPH_H * 1},

    {"W", GLYPH_W * 0, GLYPH_H * 2 },
    { "X", GLYPH_W * 1, GLYPH_H * 2 },
    { "Y", GLYPH_W * 2, GLYPH_H * 2 },
    { "Z", GLYPH_W * 3, GLYPH_H * 2 },


    {"a", GLYPH_W * 0, GLYPH_H * 3},
    {"b", GLYPH_W * 1, GLYPH_H * 3},
    {"c", GLYPH_W * 2, GLYPH_H * 3},
    {"d", GLYPH_W * 3, GLYPH_H * 3},
    {"e", GLYPH_W * 4, GLYPH_H * 3},
    {"f", GLYPH_W * 5, GLYPH_H * 3},
    {"g", GLYPH_W * 6, GLYPH_H * 3},
    {"h", GLYPH_W * 7, GLYPH_H * 3},
    {"i", GLYPH_W * 8, GLYPH_H * 3},
    {"j", GLYPH_W * 9, GLYPH_H * 3},
    {"k", GLYPH_W *10, GLYPH_H * 3},

    {"l", GLYPH_W * 0, GLYPH_H * 4},
    {"m", GLYPH_W * 1, GLYPH_H * 4},
    {"n", GLYPH_W * 2, GLYPH_H * 4},
    {"o", GLYPH_W * 3, GLYPH_H * 4},
    {"p", GLYPH_W * 4, GLYPH_H * 4},
    {"q", GLYPH_W * 5, GLYPH_H * 4},
    {"r", GLYPH_W * 6, GLYPH_H * 4},
    {"s", GLYPH_W * 7, GLYPH_H * 4},
    {"t", GLYPH_W * 8, GLYPH_H * 4},
    {"u", GLYPH_W * 9, GLYPH_H * 4},
    {"v", GLYPH_W * 10, GLYPH_H * 4},

    {"w", GLYPH_W * 0, GLYPH_H * 5 },
    { "x", GLYPH_W * 1, GLYPH_H * 5 },
    { "y", GLYPH_W * 2, GLYPH_H * 5 },
    { "z", GLYPH_W * 3, GLYPH_H * 5 },

    {"0", GLYPH_W * 0, GLYPH_H * 6},
    {"1", GLYPH_W * 1, GLYPH_H * 6},
    {"2", GLYPH_W * 2, GLYPH_H * 6},
    {"3", GLYPH_W * 3, GLYPH_H * 6},
    {"4", GLYPH_W * 4, GLYPH_H * 6},
    {"5", GLYPH_W * 5, GLYPH_H * 6},
    {"6", GLYPH_W * 6, GLYPH_H * 6},
    {"7", GLYPH_W * 7, GLYPH_H * 6},
    {"8", GLYPH_W * 8, GLYPH_H * 6},
    {"9", GLYPH_W * 9, GLYPH_H * 6},



};
#define ALPHA_GLYPH_COUNT (int)(sizeof(ALPHA_GLYPHS) / sizeof(ALPHA_GLYPHS[0]))


static const Glyph TOKUSYU_GLYPHS[] = {
    {" ", GLYPH_W * 0, GLYPH_H * 0},
    {"!", GLYPH_W * 1, GLYPH_H * 0},
    {"?", GLYPH_W * 2, GLYPH_H * 0},
    {"<", GLYPH_W * 3, GLYPH_H * 0},
    {">", GLYPH_W * 4, GLYPH_H * 0},
    {"@", GLYPH_W * 5, GLYPH_H * 0},
    {"%", GLYPH_W * 6, GLYPH_H * 0},
    {"&", GLYPH_W * 7, GLYPH_H * 0},
    {"￥", GLYPH_W * 8, GLYPH_H * 0},
    {"$", GLYPH_W * 9, GLYPH_H * 0},
    {"ー", GLYPH_W * 10, GLYPH_H * 0},

    {"女性マーク", GLYPH_W * 0, GLYPH_H * 1},
    {"男性マーク", GLYPH_W * 1, GLYPH_H * 1},
    {"+", GLYPH_W * 2, GLYPH_H * 1},
    {"-", GLYPH_W * 3, GLYPH_H * 1},
    {"*", GLYPH_W * 4, GLYPH_H * 1},
    {"/", GLYPH_W * 5, GLYPH_H * 1},
    {"=", GLYPH_W * 6, GLYPH_H * 1},

    {"←", GLYPH_W * 0, GLYPH_H * 2 },
    { "↑", GLYPH_W * 1, GLYPH_H * 2 },
    { "→", GLYPH_W * 2, GLYPH_H * 2 },
    { "↓", GLYPH_W * 3, GLYPH_H * 2 },
    {"、", GLYPH_W * 4, GLYPH_H * 2},
    {"。", GLYPH_W * 5, GLYPH_H * 2},

};
#define TOKUSYU_GLYPHS_COUNT (int)(sizeof(TOKUSYU_GLYPHS) / sizeof(TOKUSYU_GLYPHS[0]))
// ===============================
// UTF-8 1文字のバイト数
// ===============================
static int GetUTF8CharBytes(unsigned char c)
{
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// ===============================
// UTF-8 1文字を buf に抜き出す（null終端）
// ===============================
static int ExtractUTF8Char(char* outBuf5, const char* text, int i)
{
    int bytes = GetUTF8CharBytes((unsigned char)text[i]);
    outBuf5[0] = '\0';
    if (bytes <= 0) return 0;

    // 最大4バイト＋終端
    memcpy(outBuf5, &text[i], (size_t)bytes);
    outBuf5[bytes] = '\0';
    return bytes;
}

// ===============================
// Glyph 検索
// ===============================
static const Glyph* FindGlyph(const Glyph* glyphs, int count, const char* ch)
{
    for (int i = 0; i < count; i++)
    {
        if (strcmp(glyphs[i].ch, ch) == 0)
        {
            return &glyphs[i];
        }
    }
    return nullptr;
}

// ===============================
// 初期化
// ===============================
void FontDrawer_Initialize()
{
    gTexHiragana = Novice::LoadTexture("./Resource/Font/hiragana.png");
    gTexKatakana = Novice::LoadTexture("./Resource/Font/katakana.png");
    gTexAlphabet = Novice::LoadTexture("./Resource/Font/arufabeto.png");
    gTexTokusyu = Novice::LoadTexture("./Resource/Font/tokusyumozi.png");
}

// ===============================
// 文字列描画
// scale は「倍率」(1.0fで64px)
// ===============================
void DrawBitmapString(
    int x,
    int y,
    const char* text,
    float scale,
    uint32_t color)
{
    int cursorX = x;
    float scaraFontX=0;
    float scaraFontY=0;



    for (int i = 0; text[i] != '\0';)
    {
        // 改行・空白対応（必要なら増やす）
        if (text[i] == '\n')
        {
            cursorX = x;
            y += (int)(scale);
            i += 1;
            continue;
        }
        if (text[i] == ' ')
        {
            cursorX += (int)(scale);
            i += 1;
            continue;
        }

        char ch[5] = { 0 };
        int bytes = ExtractUTF8Char(ch, text, i);
        if (bytes <= 0) { break; }

        const Glyph* glyph = nullptr;
        int tex = -1;

        // ひらがな
        glyph = FindGlyph(HIRA_GLYPHS, HIRA_GLYPH_COUNT, ch);
        if (glyph)
        {
            tex = gTexHiragana;
            scaraFontX = scale / 704;
            scaraFontY = scale / 960;
        }
        else
        {
            // カタカナ
            glyph = FindGlyph(KATA_GLYPHS, KATA_GLYPH_COUNT, ch);
            if (glyph)
            {
                tex = gTexKatakana;
                scaraFontX = scale / 704;
                scaraFontY = scale / 960;
            }
            else
            {
                // 英数字

                glyph = FindGlyph(ALPHA_GLYPHS, ALPHA_GLYPH_COUNT, ch);
                if (glyph)
                {
                    tex = gTexAlphabet;
                    scaraFontX = scale / 704;
                    scaraFontY = scale / 448;
                }
                else {
                    glyph = FindGlyph(TOKUSYU_GLYPHS, TOKUSYU_GLYPHS_COUNT, ch);
                    if (glyph)
                    {
                        tex = gTexTokusyu;
                        scaraFontX = scale / 704;
                        scaraFontY = scale / 320;
                    }
                   
                }
            }
        }

        // 見つかったら描画
        if (glyph && tex >= 0)
        {
            Novice::DrawSpriteRect(
                cursorX,
                y,
                glyph->sx,
                glyph->sy,
                GLYPH_W,
                GLYPH_H,
                tex,
                scaraFontX,
                scaraFontY,
                0.0f,
                color
            );
        }

        // 次の文字位置へ（等幅）
        cursorX += (int)(scale);
        i += bytes;
    }
}
