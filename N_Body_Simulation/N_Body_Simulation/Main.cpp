

#ifndef WIN32 // if using windows then do windows specific stuff.
#define WIN32_LEAN_AND_MEAN // remove MFC overhead from windows.h which can cause slowness
#define WIN32_EXTRA_LEAN

#include <Windows.h>
#endif


#include <conio.h>
#include <iostream>

// glut includes
#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>


// my class includes
#include "Application.h"
#include "Benchmarker.h"
#include "AccuracyTester.h"
#include "BarnesHutAccuracyTester.h"
#include "Input.h"
#include "SETTINGS.h"


Application* app;
Input* input;


// define target frameTime and maximum frame time
const float kTargetFrameTime = 1.0f / 60.0f;
const float kTooLargeFrameTime = 1.0f / 10.0f;


// total time since start of simulation
float t = 0.0f;

// delta time each physics step
const float dt = 1.0f / STEPS_PER_SECOND;


// accumulator used to determine when a physics step is calculated
float accumulator = 0.0f;


// time of clock last frame
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

void MainLoop() {

	// Get time this frame
	float frameTime = GetFrameTime();

	// If frame time is above the maximum then set it to the target
	if (frameTime > kTooLargeFrameTime) {
		frameTime = kTargetFrameTime;
	}

	if (!app->GetPaused()) {
		// add frametime to accumulator
		accumulator += frameTime;


		// while the accumulator is larger than delta time a physics step needs to be calculated
		while (accumulator >= dt) {

			// perform a physics step
			app->SimulationStep(t, dt * SIMULATION_SPEED);

			// reduce the accumulator by delta time and increase the total time
			accumulator -= dt;
			t += dt * SIMULATION_SPEED;
		}

	}
	// Update the application
	app->Update(frameTime);


	// clear the render target
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// If application is interpolating between frames
	if (INTERPOLATE_BETWEEN_FRAMES) {

		// calculate how close current time is to next physics step
		const float alpha = accumulator / dt;

		// render application using alpha calculated
		app->Render(alpha);
	}
	else {
		
		// not interpolating therefore use alpha = 1 which will draw the most current position
		app->Render(1.0f);
	}


	// draw to screen
	glutSwapBuffers();
}

// Method to resize window
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

	// set buffer clear colour
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

	// setup glut functions
	glutDisplayFunc(MainLoop);
	glutReshapeFunc(Resize);
	glutIdleFunc(MainLoop);


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


	input = new Input();

#if BENCHMARKING

#if TESTING_FOR == 0
	Benchmarker benchmarker;
	
	benchmarker.Init(input);
	benchmarker.MainLoop();

#elif TESTING_FOR == 1
	AccuracyTester accuracyTester;

	accuracyTester.Init(input);
	accuracyTester.MainLoop();

#elif TESTING_FOR == 2
	BarnesHutAccuracyTester accuracyTester;

	accuracyTester.Init(input);
	accuracyTester.MainLoop();

#endif

	std::cout << "Benchmarking finished" << std::endl;

	std::cout << "press any key to exit...";

	_getch();

#else


	// Initialise glut and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glutCreateWindow("N body boiz");

	// setup glut functions
	Setup();

	// create Input and application
	app = new Application();
	app->Init(input);

	// enter main loop
	glutMainLoop();
#endif

	return 0;
}

