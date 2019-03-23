#include "Input.h"

#include <cstring>


void Input::Update() {


	std::memcpy(prev_, keys_, 256 * sizeof(bool));

}



bool Input::OnKeyPressed(int key) {

	// if key pressed this frame and not last frame
	if (keys_[key] && !prev_[key]) {

		return true;
	}


	return false;
}

bool Input::OnKeyReleased(int key) {

	// if key not pressed this frame and was pressed last frame
	if (!keys_[key] && prev_[key]) {

		return true;
	}

	return false;
}


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


