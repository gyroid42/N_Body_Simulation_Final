#pragma once


#ifndef INPUT_H
#define INPUT_H


class Input
{

	struct Mouse
	{
		int pos[2];
		bool button[2];
	};


public:
	
	void SetKeyDown(unsigned char key);
	void SetKeyUp(unsigned char key);
	bool IsKeyDown(int key);

	void SetMouseX(int new_x);
	void SetMouseY(int new_y);
	void SetMousePos(int new_x, int new_y);
	int GetMouseX();
	int GetMouseY();
	void SetMouseButton(int button, bool state);
	//void SetLeftMouseButton(bool b);
	//void SetRightMouseButton(bool b);
	bool IsMouseButtonPressed(int button);
	//bool IsLeftMouseButtonPressed();
	//bool IsRightMouseButtonPressed();

private:

	bool keys_[256];
	Mouse mouse_;
};

#endif