#pragma once


// Include Glut stuffs
#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>


// Include my classes
#include "Camera.h"


// Forward Declarations
class Simulation;
class Input;


class Application
{
public:
	Application();
	~Application();

	void Init(Input* newInput);


	// Update called each frame
	bool Update(float frameTime);

	// SimulationStep called each time a physics step happens
	bool SimulationStep(float t, float dt);

	// Method to render the simulation
	bool Render(float alpha);


	// Resizes the screen when the user resizes the window
	void Resize(int w, int h);

protected:

	// Method to check user input
	void CheckInput(float frameTime);


	void DisplayText(float x, float y, float r, float g, float b, char* string);

	// variables to construct projection matrix
	int width_, height_;
	float fov_, nearPlane_, farPlane_;

	// User Camera
	Camera camera_;

	// Input handles user input each frame
	Input* input_;

	// Simulation is the object which controls the whole n bodies simulation
	Simulation* simulation_;


	char fps_[40];
};

