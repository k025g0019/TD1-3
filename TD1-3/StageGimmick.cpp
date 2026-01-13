#include "StageGimmick.h"

#include <Novice.h>

#include "Map.h"
#include "Player.h"

void Drawn::initialize() {
	for (int i = 0; i < 100; i++) {
		drawnStatus_[i].isActive = false;
		drawnStatus_[i].pos = { -100.0f, -100.0f };
		drawnStatus_[i].height = 10.0f;
		drawnStatus_[i].width = 20.0f;

	}
}
void Drawn::update() {
}
void Drawn::draw() {
	for (int i = 0; i < 100; i++) {
		if (drawnStatus_[i].isActive) {
			
		}
	}
}
Drawn::Drawn() {
}
Drawn::~Drawn() {
}
