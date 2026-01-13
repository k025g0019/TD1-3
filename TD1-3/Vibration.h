#pragma once
#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <vector>

enum VibrationPattern {
    PATTERN_NONE = 0,
    PATTERN_METAN_EXPLOSION,
    PATTERN_GIANT_LANDING,
    PATTERN_TACKLE_HIT_WALL,
    PATTERN_ICE_METEOR_HIT,
    PATTERN_ICE_BREATH_HIT,
    PATTERN_CLAW_PUNCH,
    PATTERN_ICE_SCATTER_HIT,
    PATTERN_FALL_HOLE,
    PATTERN_EXPLOSION_DAMAGE,
    PATTERN_PICKAXE_BREAK,
    PATTERN_GIANT_FOOTSTEP,
    PATTERN_METAN_SUCTION,
    PATTERN_BOSS_COLLAPSE,
    PATTERN_PLAYER_DEATH
};

struct VibFrame {
    WORD left;
    WORD right;
    int frames;
};

class Vibration {
public:
    Vibration(int index = 0);

    void Update();                // ★毎フレーム呼ぶ
    void runPattern(VibrationPattern p);
    void stop();

private:
    int index;
    int timer;
    std::vector<VibFrame> queue;  // 振動ステップキュー

    void play(WORD left, WORD right, int frames);
    void loadPattern(VibrationPattern p);
};
