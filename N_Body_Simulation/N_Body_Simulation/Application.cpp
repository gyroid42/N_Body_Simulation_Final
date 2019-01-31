#include "Application.h"
#include "Input.h"
#include "Simulation.h"
#include "BruteForce.h"


Application::Application()
{
}


Application::~Application()
{
}





void Application::Init(Input* newInput) {

	input_ = newInput;

	// OpenGL setttings
	glShadeModel(GL_SMOOTH);
	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// create camera
	camera_.Init(input_);

	// create renderer
	// create simulation poop
	// start threads

	simulation_ = new BruteForce();
	simulation_->Init();
	simulation_->Reset();
}


void Application::CleanUp() {

	// delete resources
}


bool Application::Update(float frameTime) {


	CheckInput(frameTime);
	// check events
	camera_.Update(frameTime);
	// check inputs
	// update camera

	return true;
}

void Application::CheckInput(float frameTime) {

	if (input_->IsKeyDown('r')) {

		simulation_->Reset();
		input_->SetKeyUp('r');
	}

	if (input_->IsKeyDown(VK_ESCAPE)) {
		exit(0);
	}
}

bool Application::SimulationStep(float t, float dt) {

	// do a time step

	simulation_->TimeStep(dt);

	return true;
}


bool Application::Render(float alpha) {

	// render the poops
	// draw all the things

	
	glLoadIdentity();

	camera_.SetGluLookAt();


	//glutSolidSphere(3, 20, 20);

	simulation_->Render(alpha);

	//glBegin(GL_TRIANGLES);
	//	glVertex3f(-2, -2, -5.0f);
	//	glVertex3f(-2, 0.0, -5.0f);
	//	glVertex3f(0.0, 2, -5.0f);
	//glEnd();

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

