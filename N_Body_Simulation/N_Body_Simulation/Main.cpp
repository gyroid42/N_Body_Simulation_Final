



#ifndef WIN32 // if using windows then do windows specific stuff.
#define WIN32_LEAN_AND_MEAN // remove MFC overhead from windows.h which can cause slowness
#define WIN32_EXTRA_LEAN

#include <Windows.h>
#endif



#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>



#include "Application.h"
#include "Input.h"
#include "SETTINGS.h"


Application* app;
Input* input;



const float kTargetFrameTime = 1.0f / 60.0f;
const float kTooLargeFrameTime = 1.0f / 10.0f;


float t = 0.0f;
const float dt = 1.0f / STEPS_PER_SECOND;


float accumulator = 0.0f;


UINT64 clock_last_frame;


float GetFrameTime() {

	// calculate the time between updates
	UINT64 clock, clock_frequency;
	if (clock_last_frame == 0) {
		QueryPerformanceCounter((LARGE_INTEGER*)&clock_last_frame);
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&clock);
	QueryPerformanceFrequency((LARGE_INTEGER*)&clock_frequency);
	UINT32 cycles = (UINT32)(clock - clock_last_frame);
	clock_last_frame = clock;

	return (float)cycles / (float)clock_frequency;
}

void Render() {


	float frameTime = GetFrameTime();

	if (frameTime > kTooLargeFrameTime) {
		frameTime = kTargetFrameTime;
	}

	accumulator += frameTime;

	while (accumulator >= dt) {

		app->SimulationStep(t, dt * SIMULATION_SPEED);
		accumulator -= dt;
		t += dt * SIMULATION_SPEED;
	}

	app->Update(frameTime);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (INTERPOLATE_BETWEEN_FRAMES) {

		const float alpha = accumulator / dt;
		app->Render(alpha);
	}
	else {
		
		app->Render(1.0f);
	}


	glutSwapBuffers();
}

void Resize(int w, int h) {

	if (app) {

		app->Resize(w, h);
	}
}





//////// INPUT POOP /////////////

void ProcessNormalKeys(unsigned char key, int x, int y) {

	// If the ESCAPE key was pressed, exit application
	if (key == VK_ESCAPE) {
		exit(0);
	}

	input->SetKeyDown(key);
}


void ProcessNormalKeysUp(unsigned char key, int x, int y) {

	input->SetKeyUp(key);
}



void ProcessActiveMouseMove(int x, int y) {

	input->SetMousePos(x, y);
}

void ProcessPassiveMouseMove(int x, int y) {

	input->SetMousePos(x, y);
}

void ProcessMouseButtons(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) {

		button = 0;
	}
	else if (button == GLUT_RIGHT_BUTTON) {

		button = 1;
	}
	else {

		button = 0;
	}

	if (state == GLUT_DOWN) {

		input->SetMouseButton(button, true);
	}
	else {

		input->SetMouseButton(button, false);
	}

}



void Setup() {

	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);


	glutDisplayFunc(Render);
	glutReshapeFunc(Resize);
	glutIdleFunc(Render);


	// input callback functions
	glutKeyboardFunc(ProcessNormalKeys);
	glutKeyboardUpFunc(ProcessNormalKeysUp);


	// mouse callbacks
	glutMotionFunc(ProcessActiveMouseMove);
	glutPassiveMotionFunc(ProcessPassiveMouseMove);
	glutMouseFunc(ProcessMouseButtons);
	

}


int main(int argc, char *argv[])
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow("N body boiz");

	Setup();



	input = new Input();
	app = new Application();
	app->Init(input);

	glutMainLoop();


	return 0;
}

