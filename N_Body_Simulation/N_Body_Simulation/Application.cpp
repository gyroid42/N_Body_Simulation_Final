#include "Application.h"



Application::Application()
{
}


Application::~Application()
{
}





void Application::Init() {

	// create camera
	// create renderer
	// create simulation poop
	// start threads

}


void Application::CleanUp() {

	// delete resources
}


bool Application::Update(float frameTime) {

	// check events
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


	glColor3f(0.0f, 0.0f, 0.0f);
	

	glutSolidSphere(0.11, 20, 20);

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

