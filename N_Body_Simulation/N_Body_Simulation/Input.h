#pragma once

#ifndef INPUT_H
#define INPUT_H


class Input
{

	// struct storing mouse data
	struct Mouse
	{
		int pos[2];
		bool button[2];
	};


public:
	
	// Keyboard controls
	void SetKeyDown(unsigned char key);
	void SetKeyUp(unsigned char key);
	bool IsKeyDown(int key);

	// Mouse Controls
	void SetMouseX(int new_x);
	void SetMouseY(int new_y);
	void SetMousePos(int new_x, int new_y);
	int GetMouseX();
	int GetMouseY();
	void SetMouseButton(int button, bool state);
	bool IsMouseButtonPressed(int button);

private:

	// stores all the keyboard input data
	bool keys_[256];

	// stores mouse input data
	Mouse mouse_;
};

#endif