#include "Simulation.h"
#include "Body.h"
#include <random>
#include <ctime>
//#include "Renderer.h"

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>


Simulation::Simulation() :
	bodyCount_(0)
{
}


Simulation::~Simulation()
{
}



void Simulation::Init() {

	// get seed for random number generator
	srand(time(NULL));

	// set body count to number of random bodies
	bodyCount_ = NUM_RAND_BODIES;
}

void Simulation::CleanUp() {

	// delete all the bodies in teh simulation
	CleanUpBodies();
}

void Simulation::CleanUpBodies() {

	// loop for each body and delete it
	for (auto body : bodies_) {

		delete body;
		body = nullptr;
	}

	// clear the body list
	bodies_.clear();
}


bool Simulation::GenerateAsteroids(int numAsteroids) {

	// If adding random bodies
	if (ADD_RANDOM_BODIES) {

		
		float lower_bound = 0.0;
		float upper_bound = 700.0;
		std::uniform_real_distribution<float> unif(lower_bound, upper_bound);
		std::default_random_engine re;


		for (int i = 0; i < NUM_RAND_BODIES; i++) {

			// create new body
			Body* newAsteroid = new Body();

			// generate random position
			sf::Vector3f asteroidPos(
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_X)),
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_Y)),
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_Z))
			);

			// Initialise the new body
			newAsteroid->Init(asteroidPos, sf::Vector3f(0.0f, 0.0f, 0.0f), RANDOM_BODY_MASS);

			// Set default colour
			newAsteroid->SetColour(sf::Vector3f(0.0f, 0.0f, 1.0f));

			// Add to bodies list
			bodies_.push_back(newAsteroid);
		}

	}
	bodyCount_ = numAsteroids;
	
	
	if (ADD_ORBIT_BODIES) {

		Body* planet = new Body();
		planet->Init(sf::Vector3f(0.0f, 0.0f, 0.0f), sf::Vector3f(40.0f, 40.0f, 40.0f), 1.0E17f);
		planet->SetColour(sf::Vector3f(1.0f, 0.0f, 0.0f));
		bodies_.push_back(planet);

		Body* satelite = new Body();
		satelite->Init(sf::Vector3f(0.0f, 0.0f, -100.0f), sf::Vector3f(40.0f, -258.3215f + 40.0f, 40.0f), 100);
		satelite->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite);

		Body* satelite2 = new Body();
		satelite2->Init(sf::Vector3f(-300.0f, 0.0f, 0.0f), sf::Vector3f(40.0f, -149.14199f + 40.0f, 40.0f), 100);
		satelite2->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite2);


		Body* satelite3 = new Body();
		satelite3->Init(sf::Vector3f(0.0f, -1005.0f, 0.0f), sf::Vector3f(-115.5249f + 40.0f, 40.0f, 40.0f), 100);
		satelite3->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite3);

		Body* satelite4 = new Body();
		satelite4->Init(sf::Vector3f(0.0f, -1000.0f, 0.0f), sf::Vector3f(81.68843f + 40.0f, 40.0f, 40.0f), 100);
		satelite4->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite4);

	}

	return true;
}


void Simulation::AddBody(Body* newBody) {

	//newBody->SetColour(sf::Color::Green);

	// add new body to body list
	bodies_.push_back(newBody);
}


bool Simulation::Reset() {

	// delete all teh bodies in the simulation
	CleanUpBodies();


	// generate a new simulation
	if (!GenerateAsteroids(bodyCount_)) {

		return false;
	}


	return true;
}


// Virtual method
void Simulation::TimeStep(float dt) {


	
}


void Simulation::Render(float alpha) {

	// loop for each body and draw it
	for (auto body : bodies_) {

		body->Draw(alpha);
		
	}
}


