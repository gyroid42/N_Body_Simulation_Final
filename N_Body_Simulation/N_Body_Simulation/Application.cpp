
// include class header
#include "Application.h"

// my class includes
#include "Input.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "BarnesHutCPU.h"
#include "Semaphore.h"
#include "PhysicsUtil.h"

#include <iostream>
#include <chrono>
#include <ctime>

typedef std::chrono::system_clock the_clock;


Application::Application() :
	simulation_(nullptr),
	input_(nullptr),
	simSettings_(nullptr),
	physicsThread_(nullptr),
	timeStepCounter_(nullptr)
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

	if (physicsThread_) {

		running_ = false;
		timeStepCounter_->Signal();
		physicsThread_->join();
		
	}
}





void Application::Init(Input* newInput) {

	// get reference to input
	input_ = newInput;

	running_ = true;
	paused_ = false;

	// OpenGL settings
	glShadeModel(GL_SMOOTH);
	//glClearColor(0.39f, 0.58f, 93.0f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// create camera
	camera_.Init(input_);



	SimulationSettings newSimSettings;
	newSimSettings.collision = false;
	newSimSettings.simMethod = Barnes_Hut;
	newSimSettings.integrationMethod = Semi_Implicit_Euler;
	newSimSettings.simMode = Even_Distribution;
	newSimSettings.bodyCount = 3000;
	newSimSettings.dt = 1.0f / 20.0f;
	newSimSettings.theta = 1.0f;
	newSimSettings.orderBodies = true;
	newSimSettings.threadCount = 8;

	newSimSettings.planetMass = 1.0E+17f;
	newSimSettings.satelliteMass = 100.0f;
	newSimSettings.orbitStartPos = sf::Vector3f(0.0f, 100.0f, 0.0f);

	float orbitDistance = PhysicsUtil::VectorLength(newSimSettings.orbitStartPos);

	newSimSettings.orbitStartVel = sf::Vector3f(0.0f, 0.0f, sqrtf(PhysicsUtil::G * newSimSettings.planetMass / orbitDistance));

	newSimSettings.numPeriods = 100;
	newSimSettings.periodLength = 2.0f * PhysicsUtil::pi * orbitDistance * sqrtf(orbitDistance) / sqrtf(PhysicsUtil::G * newSimSettings.planetMass);

	// Create and start simulation
	switch (newSimSettings.simMethod) {
	case Direct:
		simulation_ = new BruteForce();
		break;
	case Barnes_Hut:
		simulation_ = new BarnesHutCPU();
		break;
	default:
		simulation_ = new BruteForce();
		break;
	}

	simulation_->NewSettings(newSimSettings);
	simulation_->Init();
	simulation_->Reset();
	simSettings_ = simulation_->Settings();


	simMode_ = simSettings_->simMode;


	UpdateUIText();
	UpdateSimUIText();
	//textColour_ = { 0.0f, 0.0f, 0.0f };
	textColour_ = { 1.0f, 1.0f, 1.0f };


	timeAccumulator_ = 0.0f;
	timeStepCounter_ = new Semaphore();

	renderAccumulator_ = 0.0f;

	//physicsThread_ = new std::thread(std::mem_fun(&Application::FixedUpdate), this, simSettings_->dt);

}


bool Application::Update(float frameTime) {


	timeAccumulator_ += frameTime;
	renderAccumulator_ += frameTime;


	while (timeAccumulator_ >= simSettings_->dt) {


		//simulation_->ShiftBodyStates();


		//timeStepCounter_->Signal();
		timeAccumulator_ -= simSettings_->dt;
	}

	// Check user input
	CheckInput(frameTime);

	// Update the camera
	camera_.Update(frameTime);

	sprintf_s(textUI_.fps, "FPS: %4.2f", 1.0f / frameTime);
	sprintf_s(textUI_.renderTime, "Render Time: %4.2f", simulation_->RenderTime());

	input_->Update();

	return true;
}



void Application::FixedUpdate(float dt) {

	
	while (running_) {

		timeStepCounter_->Wait();

		if (!running_) {

			break;
		}
		simulation_->TimeStep(dt);

		UpdateSimUIText();
	}
}



void Application::CheckInput(float frameTime) {

	// Reset the simulation when 'r' is pressed
	if (input_->OnKeyPressed('r')) {

		simulation_->Reset();
	}

	if (input_->OnKeyPressed('p')) {

		paused_ = !paused_;
	}


	if (input_->OnKeyPressed('m')) {

		switch (simMode_) {
		case Random_Bodies:
			simMode_ = Two_Body_Orbit;
			break;
		case Two_Body_Orbit:
			simMode_ = Even_Distribution;
			break;
		case Even_Distribution:
			simMode_ = Random_Bodies;
			break;
		default:
			simMode_ = Random_Bodies;
			break;
		}

		UpdateSimModeText();
	}

	// exit the application if escape is pressed
	if (input_->OnKeyPressed(VK_ESCAPE)) {

		exit(0);
	}
}

bool Application::SimulationStep(float t, float dt) {

	// do a time step in the simulation
	simulation_->TimeStep(dt);

	UpdateSimUIText();


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
	//simulation_->Render(timeAccumulator_ / simSettings_->dt);
	simulation_->Render(alpha);

	if (renderAccumulator_ > 1.0f) {
		int mejklre = 0;
	}

	DisplayText(0.9f, 0.96f, textUI_.fps);

	DisplayText(-1.f, 0.96f, textUI_.simMode);

	DisplayText(-1.f, 0.88f, textUI_.simMethod);
	DisplayText(-1.f, 0.84f, textUI_.integrationMethod);
	DisplayText(-1.f, 0.8f, textUI_.timingSteps);

	DisplayText(-1.f, 0.72f, textUI_.multiThreading);
	DisplayText(-1.f, 0.68f, textUI_.threadCount);

	DisplayText(-1.f, 0.60f, textUI_.collision);
	DisplayText(-1.f, 0.56f, textUI_.maxCollisionTreeDepth);


	{

		std::unique_lock<std::mutex> lock(simUI_Mutex_);

		DisplayText(-1.f, 0.48f, textUI_.bodyCount);


		DisplayText(-1.f, 0.4f, textUI_.calcTimeTitle);
		DisplayText(-1.f, 0.36f, textUI_.timeForInsertion);
		DisplayText(-1.f, 0.32f, textUI_.timeForForce);
		DisplayText(-1.f, 0.28f, textUI_.integrationTime);
		DisplayText(-1.f, 0.24f, textUI_.sortTime);
		DisplayText(-1.f, 0.20f, textUI_.collisionTime);


	}

	DisplayText(-1.f, 0.12f, textUI_.renderTime);

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




void Application::DisplayText(float x, float y, float r, float g, float b, char* string) {

	// Get Lenth of string
	int j = strlen(string);

	// Swap to 2D rendering
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Orthographic lookAt (along the z-axis).
	gluLookAt(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Set text colour and position.
	glColor3f(r, g, b);
	glRasterPos2f(x, y);
	// Render text.
	for (int i = 0; i < j; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
	}
	// Reset colour to white.
	glColor3f(1.f, 1.f, 1.f);

	// Swap back to 3D rendering.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov_, ((float)width_ / (float)height_), nearPlane_, farPlane_);
	glMatrixMode(GL_MODELVIEW);
}

void Application::DisplayText(float x, float y, sf::Vector3f colour, char* string) {

	DisplayText(x, y, colour.x, colour.y, colour.z, string);
}

void Application::DisplayText(float x, float y, char* string) {

	DisplayText(x, y, textColour_, string);
}


void Application::UpdateUIText() {

	UpdateSimModeText();
	UpdateSimMethodText();
	UpdateIntegrationMethodText();

	//sprintf_s(textUI_.bodyCount, "Body Count = %d", simulation_->BodyCount());
	sprintf_s(textUI_.multiThreading, "Multi-threading = %s", (simSettings_->multiThreading) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.collision, "Collision = %s", (simSettings_->collision) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.timingSteps, "Timing Steps = %s", (simSettings_->timingSteps) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.maxCollisionTreeDepth, "Max Collision Tree Depth = %d", simSettings_->maxCollisionDepth);
	sprintf_s(textUI_.threadCount, "Thread Count = %d", simSettings_->threadCount);

	sprintf_s(textUI_.calcTimeTitle, "Step Calculation Times");

}

void Application::UpdateSimUIText() {

	std::unique_lock<std::mutex> lock(simUI_Mutex_);

	sprintf_s(textUI_.bodyCount, "Body Count = %d", simulation_->BodyCount());
	sprintf_s(textUI_.timeForInsertion, "Insert Time: %4.2f", simulation_->InsertTime());
	sprintf_s(textUI_.timeForForce, "Force Time: %4.2f", simulation_->ForceTime());
	sprintf_s(textUI_.integrationTime, "Integration Time: %4.2f", simulation_->IntegrationTime());
	sprintf_s(textUI_.sortTime, "Sort Time: %4.2f", simulation_->SortTime());
	sprintf_s(textUI_.collisionTime, "Collision Time: %4.2f", simulation_->CollisionTime());

}

void Application::UpdateSimModeText() {

	switch (simMode_) {
	case Random_Bodies:
		sprintf_s(textUI_.simMode, "Simulation Mode = Random Bodies");
		break;
	case Two_Body_Orbit:
		sprintf_s(textUI_.simMode, "Simulation Mode = 2-Body Orbit");
		break;
	case Even_Distribution:
		sprintf_s(textUI_.simMode, "Simulation Mode = Even Distribution");
		break;
	case Clustered_Distribution:
		sprintf_s(textUI_.simMode, "Simulation Mode = Cluster Distribution");
		break;
	default:
		sprintf_s(textUI_.simMode, "Simulation Mode = UNKNOWN");
		break;
	}
}

void Application::UpdateSimMethodText() {

	switch (simSettings_->simMethod) {
	case Direct:
		sprintf_s(textUI_.simMethod, "Simulation Method = Direct");
		break;
	case Barnes_Hut:
		sprintf_s(textUI_.simMethod, "Simulation Method = Barnes-Hut");
		break;
	default:
		sprintf_s(textUI_.simMethod, "Simulation Method = UNKNOWN");
		break;
	}
}

void Application::UpdateIntegrationMethodText() {

	switch (simSettings_->integrationMethod) {
	case Semi_Implicit_Euler:
		sprintf_s(textUI_.integrationMethod, "Integration Method = Semi-Implicit Euler");
		break;
	case Verlet:
		sprintf_s(textUI_.integrationMethod, "Integration Method = Verlet");
		break;
	default:
		sprintf_s(textUI_.integrationMethod, "Integration Method = UNKNOWN");
		break;
	}
}


void Application::ReduceAccumulator(float dt) {

	std::unique_lock<std::mutex> lock(accMutex_);
	renderAccumulator_ -= timeAccumulator_;
}


