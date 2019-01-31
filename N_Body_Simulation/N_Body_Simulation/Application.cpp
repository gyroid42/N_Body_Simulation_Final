#include "Application.h"
#include "Input.h"


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

}


void Application::CleanUp() {

	// delete resources
}


bool Application::Update(float frameTime) {

	// check events
	camera_.Update(frameTime);
	// check inputs
	// update camera

	return true;
}

bool Application::SimulationStep(float t, float dt) {

	// do a time step

	return true;
}


bool Application::Render(float alpha) {

	// render the poops
	// draw all the things


	glColor3f(1.0f, 0.0f, 0.0f);
	
	glLoadIdentity();

	camera_.SetGluLookAt();


	glutSolidSphere(3, 20, 20);

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
	farPlane_ = 100.0f;

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glViewport(0, 0, w, h);

	gluPerspective(fov_, ratio, nearPlane_, farPlane_);

	glMatrixMode(GL_MODELVIEW);
}

