#pragma once

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "Camera.h"

class Input;

class Application
{
public:
	Application();
	~Application();


	void Init(Input* newInput);
	void CleanUp();


	bool Update(float frameTime);
	bool SimulationStep(float t, float dt);

	bool Render(float alpha);

	void Resize(int w, int h);

protected:



	int width_, height_;
	float fov_, nearPlane_, farPlane_;

	Camera camera_;
	Input* input_;

};

