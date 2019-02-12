
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
	//glShadeModel(GL_SMOOTH);
	//glClearColor(0.39f, 0.58f, 93.0f, 1.0f);
	//glClearDepth(1.0f);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	sprintf_s(fps_, "FPS: %4.2f", 1.0f / frameTime);

	return true;
}

void Application::CheckInput(float frameTime) {

	// Reset the simulation when 'r' is pressed
	if (input_->IsKeyDown('r')) {

		simulation_->Reset();
		input_->SetKeyUp('r');
	}

	// exit the application if escape is pressed
	//if (input_->IsKeyDown(VK_ESCAPE)) {

	//	exit(0);
	//}
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
	//glLoadIdentity();

	// Update glut lookat using camera
	camera_.SetGluLookAt();

	// Render the simulation
	simulation_->Render(alpha);

	DisplayText(-1.f, 0.96f, 1.f, 0.f, 0.f, fps_);

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

	//glMatrixMode(GL_PROJECTION);

	//glLoadIdentity();

	//glViewport(0, 0, w, h);

	//gluPerspective(fov_, ratio, nearPlane_, farPlane_);

	//glMatrixMode(GL_MODELVIEW);
}





void Application::DisplayText(float x, float y, float r, float g, float b, char* string) {

	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	//gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	//glColor3f(r, g, b);
	//glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
	//	glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	//glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(fov_, ((float)width_ / (float)height_), nearPlane_, farPlane_);
	//glMatrixMode(GL_MODELVIEW);
}



