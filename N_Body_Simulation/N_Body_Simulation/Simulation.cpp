#include "Simulation.h"


#include "Body.h"
#include "Application.h"

#include <random>
#include <ctime>
#include <iostream>

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include <chrono>

typedef std::chrono::steady_clock the_clock;


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
	bodyCount_ = settings_.numRandBodies;
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


bool Simulation::GenerateRandom() {

	// If adding random bodies
	float lower_bound = 0.0;
	float upper_bound = 700.0;
	std::uniform_real_distribution<float> unif(lower_bound, upper_bound);
	std::default_random_engine re;


	for (int i = 0; i < settings_.bodyCount; i++) {

		// create new body
		Body* newAsteroid = new Body();

		// generate random position
		sf::Vector3f asteroidPos(
			static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / settings_.maxRandX)),
			static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / settings_.maxRandY)),
			static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / settings_.maxRandZ))
		);

		// Initialise the new body
		newAsteroid->Init(asteroidPos, sf::Vector3f(0.0f, 0.0f, 0.0f), settings_.randBodyMass, settings_.integrationMethod);

		// Set default colour
		newAsteroid->SetColour(sf::Vector3f(0.0f, 0.0f, 1.0f));

		// Add to bodies list
		bodies_.push_back(newAsteroid);
	}

	if (ADD_ORBIT_BODIES) {

		Body* planet = new Body();
		planet->Init(sf::Vector3f(0.0f, 0.0f, 0.0f), sf::Vector3f(50.0f, 50.0f, 50.0f), 1.0E17f, settings_.integrationMethod);
		planet->SetColour(sf::Vector3f(1.0f, 0.0f, 0.0f));
		bodies_.push_back(planet);

		Body* satelite = new Body();
		satelite->Init(sf::Vector3f(0.0f, 0.0f, -100.0f), sf::Vector3f(50.0f, -258.3215f + 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite);

		Body* satelite2 = new Body();
		satelite2->Init(sf::Vector3f(-300.0f, 0.0f, 0.0f), sf::Vector3f(50.0f, -149.14199f + 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite2->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite2);


		Body* satelite3 = new Body();
		satelite3->Init(sf::Vector3f(0.0f, -1005.0f, 0.0f), sf::Vector3f(-115.5249f + 50.0f, 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite3->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite3);

		Body* satelite4 = new Body();
		satelite4->Init(sf::Vector3f(0.0f, -1000.0f, 0.0f), sf::Vector3f(81.68843f + 50.0f, 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite4->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite4);

	}

	return true;
}



bool Simulation::GenerateGrid() {


	int gridLength = std::cbrtf((float)settings_.bodyCount);
	int remainder = settings_.bodyCount % (gridLength * gridLength * gridLength);

	float spacing = (settings_.partitionSize - 4000.0f) / (float)gridLength;

	sf::Vector3f startPos(2000.0f, 2000.0f, 2000.0f);

	for (int x = 0; x < gridLength; x++) {

		for (int y = 0; y < gridLength; y++) {

			for (int z = 0; z < gridLength + (remainder > 0) ? 1 : 0; z++) {

				// create new body
				Body* newBody = new Body();

				// generate random position
				sf::Vector3f bodyPos(x, y, z);

				bodyPos *= spacing;
				bodyPos += startPos;

				// Initialise the new body
				newBody->Init(bodyPos, sf::Vector3f(0.0f, 0.0f, 0.0f), settings_.randBodyMass, settings_.integrationMethod);

				// Set default colour
				newBody->SetColour(sf::Vector3f(0.0f, 0.0f, 1.0f));

				// Add to bodies list
				bodies_.push_back(newBody);
			}

			remainder--;
		}
	}


	return true;
}


bool Simulation::GenerateClusters() {


	int bodyIndex = 0;
	
	// split the number of bodies into 8
	int numBodiesCluster = settings_.bodyCount / 8;

	int remainder = settings_.bodyCount % (numBodiesCluster);

	sf::Vector3f clusterStartPos(2000.0f, 2000.0f, 2000.0f);
	float clusterSpacing = 4000.0f;


	int bodyGridLength = std::cbrtf((float)numBodiesCluster);
	float bodySpacing = 2000.0f / (float)bodyGridLength;

	for (int i = 0; i < 8; i++) {


		bool extraBody = ((remainder > 0) ? true : false);
		remainder--;

		sf::Vector3f offset;

		offset.x = ((i & 1) ? clusterSpacing : 0.0f);
		offset.y = ((i & 2) ? clusterSpacing : 0.0f);
		offset.z = ((i & 4) ? clusterSpacing : 0.0f);

		sf::Vector3f startPos = clusterStartPos + offset;

		for (int x = 0; x < bodyGridLength; x++) {

			for (int y = 0; y < bodyGridLength; y++) {

				for (int z = 0; z < bodyGridLength + ((extraBody) ? 1 : 0); z++) {

					// create new body
					Body* newBody = new Body();

					// generate random position
					sf::Vector3f bodyPos(x, y, z);

					bodyPos *= bodySpacing;
					bodyPos += startPos;

					// Initialise the new body
					newBody->Init(bodyPos, sf::Vector3f(0.0f, 0.0f, 0.0f), settings_.randBodyMass, settings_.integrationMethod);

					// Set default colour
					newBody->SetColour(sf::Vector3f(0.0f, 0.0f, 1.0f));

					// Set body name
					newBody->SetName(std::to_string(bodyIndex));
					bodyIndex++;

					// Add to bodies list
					bodies_.push_back(newBody);
				}

				extraBody = false;
			}
		}
	}



	if (ADD_ORBIT_BODIES) {

		Body* planet = new Body();
		planet->Init(sf::Vector3f(500.0f, 500.0f, 500.0f), sf::Vector3f(50.0f, 50.0f, 50.0f), 1.0E17f, settings_.integrationMethod);
		planet->SetColour(sf::Vector3f(1.0f, 0.0f, 0.0f));
		bodies_.push_back(planet);

		Body* satelite = new Body();
		satelite->Init(sf::Vector3f(500.0f, 500.0f, 400.0f), sf::Vector3f(50.0f, -258.3215f + 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite);

		Body* satelite2 = new Body();
		satelite2->Init(sf::Vector3f(200.0f, 500.0f, 500.0f), sf::Vector3f(50.0f, -149.14199f + 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite2->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite2);


		Body* satelite3 = new Body();
		satelite3->Init(sf::Vector3f(500.0f, -505.0f, 500.0f), sf::Vector3f(-115.5249f + 50.0f, 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite3->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite3);

		Body* satelite4 = new Body();
		satelite4->Init(sf::Vector3f(500.0f, -500.0f, 500.0f), sf::Vector3f(81.68843f + 50.0f, 50.0f, 50.0f), 100, settings_.integrationMethod);
		satelite4->SetColour(sf::Vector3f(0.0f, 0.5f, 0.5f));
		bodies_.push_back(satelite4);

	}

	return true;
}


bool Simulation::GenerateAsteroidBelt() {



	return true;
}


bool Simulation::GenerateTwoBodyOrbit() {


	// create planet
	Body* planet = new Body();
	planet->Init(sf::Vector3f(0.0f, 0.0f, 0.0f), sf::Vector3f(0.0f, 0.0f, 0.0f), settings_.planetMass, settings_.integrationMethod);
	planet->SetName("planet");
	bodies_.push_back(planet);

	// create satellite
	Body* satellite = new Body();
	satellite->Init(settings_.orbitStartPos, settings_.orbitStartVel, settings_.satelliteMass, settings_.integrationMethod);
	satellite->SetName("satellite");
	bodies_.push_back(satellite);


	return true;
}


bool Simulation::GenerateSimulation(SIMULATION_MODE simMode) {


	bool result = true;

	switch (simMode) {
	case Random_Bodies: // Random Bodies
		result = GenerateRandom();
		break;
	case Two_Body_Orbit:
		result = GenerateTwoBodyOrbit();
		break;
	case Even_Distribution:
		result = GenerateGrid();
		break;
	case Clustered_Distribution:
		result = GenerateClusters();
		break;
	default:
		result = GenerateRandom();
		break;
	}

	return result;
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
	if (!GenerateSimulation(settings_.simMode)) {

		return false;
	}


	return true;
}


// Virtual method
void Simulation::TimeStep(float dt) {

	ShiftBodyStates();
	
}


void Simulation::Render(float alpha) {

	the_clock::time_point timeStart;
	the_clock::time_point timeEnd;

	timeStart = the_clock::now();

	std::unique_lock<std::mutex> lock(bodyListMutex_);
	// loop for each body and draw it
	for (auto body : bodies_) {

		body->Draw(alpha);
		
	}

	timeEnd = the_clock::now();

	std::cout << "render time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

}



Body* Simulation::GetBody(std::string bodyName) {

	for (auto body : bodies_) {

		if (body->Name() == bodyName) {

			return body;
		}
	}

	return nullptr;
}



void Simulation::ShiftBodyStates() {

	std::unique_lock<std::mutex> lock(bodyListMutex_);
	for (auto body : bodies_) {

		body->ShiftStates();
	}

}


