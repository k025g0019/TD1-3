#pragma once
#include "Vector.h"
class Player {
	public:
		struct PlayerStatus {
			Vector2 pos;
			Vector2 vel;
		};
		void Initialize();
		void Update();
		void Draw();

};