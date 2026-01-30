#include "camera.h"
#include <algorithm> // std::clamp

void Camera::Reset()
{
    x = 0.0f;
    y = 0.0f;
    shakeTimer = 0;
    shakeRange = 0.0f;
}


// ---------------------------------------------
// プレイヤー追従カメラ
// ---------------------------------------------
void Camera::Follow(float playerX, float playerY)
{
    (void)playerY;

    const float kScreenWidth = 1280.0f;
    const float kWorldWidth = 2560.0f;

    float targetX = (kScreenWidth / 2.0f) - playerX;

    const float followRate = 0.15f;
    x += (targetX - x) * followRate;

    // ---- 正しいクランプ ----
    if (x > 0.0f) {
        x = 0.0f;
    }
    if (x < -(kWorldWidth - kScreenWidth)) {
        x = -(kWorldWidth - kScreenWidth);
    }

    y = 0.0f;

    UpdateShake();
}



// ---------------------------------------------
// 振動開始
// ---------------------------------------------
void Camera::StartShake(int duration, float range) {
    shakeTimer = duration;
    shakeRange = range;
}

// ---------------------------------------------
// 振動更新
// ---------------------------------------------
void Camera::UpdateShake() {
    if (shakeTimer > 0) {
        shakeTimer--;

        float shakeX = (rand() % 1000 / 1000.0f - 0.5f) * shakeRange;
        float shakeY = (rand() % 1000 / 1000.0f - 0.5f) * shakeRange;

        x += shakeX;
        y += shakeY;
    }
}
