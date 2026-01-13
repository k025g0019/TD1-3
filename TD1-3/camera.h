#pragma once
#include <stdlib.h>
#include <time.h>

class Camera {
public:
	float x;
	float y;
	// ▼ カメラ振動用変数（既存）
	int shakeTimer;
	float shakeRange;

	// ★ シングルトン取得
	static Camera& Instance() {
		static Camera instance;
		return instance;
	}

	// ▼ プレイヤー追従（これを追加！）
	void Follow(float playerX, float playerY);

	// ▼ カメラ振動（既存）
	void StartShake(int duration, float range);
	void UpdateShake();

private:
	Camera() : x(0), y(0), shakeTimer(0), shakeRange(0) {}
};
