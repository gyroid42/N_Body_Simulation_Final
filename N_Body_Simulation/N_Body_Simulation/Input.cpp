#include "Input.h"




void Input::SetKeyDown(unsigned char key) {

	keys_[key] = true;
}

void Input::SetKeyUp(unsigned char key) {

	keys_[key] = false;
}

bool Input::IsKeyDown(int key) {

	return keys_[key];
}

void Input::SetMouseX(int new_x) {

	mouse_.pos[0] = new_x;
}

void Input::SetMouseY(int new_y) {

	mouse_.pos[1] = new_y;
}

void Input::SetMousePos(int new_x, int new_y) {

	mouse_.pos[0] = new_x;
	mouse_.pos[1] = new_y;
}

int Input::GetMouseX() {

	return mouse_.pos[0];
}

int Input::GetMouseY() {

	return mouse_.pos[1];
}

void Input::SetMouseButton(int button, bool b) {

	mouse_.button[button] = b;
}

bool Input::IsMouseButtonPressed(int button) {

	return mouse_.button[button];
}


