#pragma once
#include "raylib.h"
#include <string>
// to load an image, call Image img = LoadImage("path/to/image.png"); ImageCrop(&img, Rectangle{0, 0, 32, 32}););
class Player {

public:
	float x, y;
	int width, height;
	int speed;

	void Draw() {
		DrawRectangle(x, y, width, height, WHITE);
	}

	void MoveUp() {
		y -= speed;
		LimitMovement();
	}

	void MoveDown() {
		y += speed;
		LimitMovement();
	}
	void LimitMovement() {
		if (y <= 0) y = 0;
		if (y + height >= GetScreenHeight()) y = GetScreenHeight() - height;
	}
};

class NPC : public Player {
	public:
	void DisplayText(std::string t) {
		// Implementation of displaying text
	}
	void Speak(std::string text) {
		// Implementation of NPC speaking
		DisplayText(text);
	}
};