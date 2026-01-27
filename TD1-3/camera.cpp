#include "camera.h"

// ---------------------------------------------
// プレイヤー追従カメラ
// ---------------------------------------------
void Camera::Follow(float playerX, float playerY)
{
    (void)playerY;

    float targetX = (1280 / 2) - playerX;

    const float followRate = 0.15f;
    x += (targetX - x) * followRate;

    y = 0;

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
