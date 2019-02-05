
// include class header
#include "Application.h"

// my class includes
#include "Input.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "BarnesHutCPU.h"


Application::Application()
{
}


Application::~Application()
{

	// cleanup resources
	if (input_) {
		delete input_;
		input_ = nullptr;
	}

	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}
}





void Application::Init(Input* newInput) {

	// get reference to input
	input_ = newInput;

	// OpenGL settings
	glShadeModel(GL_SMOOTH);
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// create camera
	camera_.Init(input_);

	// Create and start simulation
	simulation_ = new BarnesHutCPU();
	simulation_->Init();
	simulation_->Reset();
}


bool Application::Update(float frameTime) {


	// Check user input
	CheckInput(frameTime);

	// Update the camera
	camera_.Update(frameTime);

	return true;
}

void Application::CheckInput(float frameTime) {

	// Reset the simulation when 'r' is pressed
	if (input_->IsKeyDown('r')) {

		simulation_->Reset();
		input_->SetKeyUp('r');
	}

	// exit the application if escape is pressed
	if (input_->IsKeyDown(VK_ESCAPE)) {

		exit(0);
	}
}

bool Application::SimulationStep(float t, float dt) {

	// do a time step in the simulation
	simulation_->TimeStep(dt);

	return true;
}


bool Application::Render(float alpha) {

	// render the poops
	// draw all the things

	// Load the identity matrix
	glLoadIdentity();

	// Update glut lookat using camera
	camera_.SetGluLookAt();

	// Render the simulation
	simulation_->Render(alpha);

	return true;
}

void Application::Resize(int w, int h) {

	width_ = w;
	height_ = h;

	if (h == 0) {
		h = 1;
	}

	float ratio = (float)w / (float)h;
	fov_ = 45.0f;
	nearPlane_ = 0.1f;
	farPlane_ = 10000.0f;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(fov_, ratio, nearPlane_, farPlane_);

	glMatrixMode(GL_MODELVIEW);
}

