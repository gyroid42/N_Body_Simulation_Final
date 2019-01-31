#pragma once

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "Camera.h"

class Simulation;

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


	void CheckInput(float frameTime);

	int width_, height_;
	float fov_, nearPlane_, farPlane_;

	Camera camera_;
	Input* input_;


	Simulation* simulation_;
};

