#pragma once
#include "Vector.h"
class Drawn {
public:
	Drawn();
	~Drawn();
	void initialize();
	void update();
	void draw();
private:
	struct DrawnStatus {
		Vector2 pos;
		bool isActive;
		float height;
		float width;

	};
	DrawnStatus drawnStatus_[100];
};