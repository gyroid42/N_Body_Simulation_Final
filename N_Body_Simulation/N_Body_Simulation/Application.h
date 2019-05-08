#pragma once


// Include standard library
#include <thread>
#include <mutex>

// Include Glut stuffs
#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>


// Include my classes
#include "Camera.h"
#include "TextUI.h"


// Forward Declarations
class Simulation;
class Input;
class Semaphore;

struct SimulationSettings;

enum SIMULATION_MODE;

class Application
{
public:
	Application();
	~Application();

	void Init(Input* newInput);


	// Update called each frame
	bool Update(float frameTime);

	// Fixed Update called at fixed interval of time
	void FixedUpdate(float dt);

	// SimulationStep called each time a physics step happens
	bool SimulationStep(float t, float dt);

	// Method to render the simulation
	bool Render(float alpha);


	// Resizes the screen when the user resizes the window
	void Resize(int w, int h);


	void ReduceAccumulator(float dt);

protected:

	// Method to check user input
	void CheckInput(float frameTime);


	void DisplayText(float x, float y, float r, float g, float b, char* string);
	void DisplayText(float x, float y, sf::Vector3f colour, char* string);
	void DisplayText(float x, float y, char* string);

	void UpdateUIText();
	void UpdateSimUIText();
	void UpdateSimModeText();
	void UpdateSimMethodText();
	void UpdateIntegrationMethodText();

	// variables to construct projection matrix
	int width_, height_;
	float fov_, nearPlane_, farPlane_;

	// User Camera
	Camera camera_;

	// Input handles user input each frame
	Input* input_;

	// Simulation is the object which controls the whole n bodies simulation
	Simulation* simulation_;

	SimulationSettings* simSettings_;


	SIMULATION_MODE simMode_;

	sf::Vector3f textColour_;
	std::mutex simUI_Mutex_;
	TextUI textUI_;


	std::thread* physicsThread_;
	Semaphore* timeStepCounter_;

	float timeAccumulator_;

	float renderAccumulator_;
	std::mutex accMutex_;

	bool running_;
};

