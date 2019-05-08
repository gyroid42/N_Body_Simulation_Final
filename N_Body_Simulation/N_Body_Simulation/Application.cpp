
// include class header
#include "Application.h"

// my class includes
#include "Input.h"
#include "Simulation.h"
#include "BruteForce.h"
#include "BarnesHutCPU.h"


Application::Application() :
	simulation_(nullptr),
	input_(nullptr),
	simSettings_(nullptr)
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



	SimulationSettings newSimSettings;
	newSimSettings.collision = true;
	newSimSettings.simMethod = Barnes_Hut;
	newSimSettings.integrationMethod = Semi_Implicit_Euler;
	newSimSettings.simMode = Clustered_Distribution;
	newSimSettings.bodyCount = 3000;
	newSimSettings.orderBodies = true;

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

}


bool Application::Update(float frameTime) {


	// Check user input
	CheckInput(frameTime);

	// Update the camera
	camera_.Update(frameTime);

	sprintf_s(textUI_.fps, "FPS: %4.2f", 1.0f / frameTime);
	sprintf_s(textUI_.bodyCount, "Body Count = %d", simulation_->BodyCount());


	input_->Update();

	return true;
}



void Application::CheckInput(float frameTime) {

	// Reset the simulation when 'r' is pressed
	if (input_->OnKeyPressed('r')) {

		simulation_->Reset();
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

	DisplayText(0.9f, 0.96f, 0.f, 0.f, 0.f, textUI_.fps);

	DisplayText(-1.f, 0.96f, 0.f, 0.f, 0.f, textUI_.simMode);

	DisplayText(-1.f, 0.88f, 0.f, 0.f, 0.f, textUI_.simMethod);
	DisplayText(-1.f, 0.84f, 0.f, 0.f, 0.f, textUI_.integrationMethod);
	DisplayText(-1.f, 0.8f, 0.f, 0.f, 0.f, textUI_.timingSteps);

	DisplayText(-1.f, 0.72f, 0.f, 0.f, 0.f, textUI_.multiThreading);
	DisplayText(-1.f, 0.68f, 0.f, 0.f, 0.f, textUI_.threadCount);

	DisplayText(-1.f, 0.60f, 0.f, 0.f, 0.f, textUI_.collision);
	DisplayText(-1.f, 0.56f, 0.f, 0.f, 0.f, textUI_.maxCollisionTreeDepth);
	DisplayText(-1.f, 0.48f, 0.f, 0.f, 0.f, textUI_.bodyCount);


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


void Application::UpdateUIText() {

	UpdateSimModeText();
	UpdateSimMethodText();
	UpdateIntegrationMethodText();

	sprintf_s(textUI_.bodyCount, "Body Count = %d", simulation_->BodyCount());
	sprintf_s(textUI_.multiThreading, "Multi-threading = %s", (simSettings_->multiThreading) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.collision, "Collision = %s", (simSettings_->collision) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.timingSteps, "Timing Steps = %s", (simSettings_->timingSteps) ? "TRUE" : "FALSE");
	sprintf_s(textUI_.maxCollisionTreeDepth, "Max Collision Tree Depth = %d", simSettings_->maxCollisionDepth);
	sprintf_s(textUI_.threadCount, "Thread Count = %d", simSettings_->threadCount);
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


