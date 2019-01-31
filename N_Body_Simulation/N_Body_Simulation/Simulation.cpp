#include "Simulation.h"
#include "Body.h"
#include <random>
#include <ctime>
//#include "Renderer.h"


Simulation::Simulation() :
	bodyCount_(0)
{
}


Simulation::~Simulation()
{
}



void Simulation::Init() {

	srand(time(NULL));

	bodyCount_ = NUM_RAND_BODIES;
}

void Simulation::CleanUp() {


}

void Simulation::CleanUpBodies() {

	for (auto body : bodies_) {

		delete body;
		body = nullptr;
	}

	bodies_.clear();

	return;
}


bool Simulation::GenerateAsteroids(int numAsteroids) {

	
	if (ADD_RANDOM_BODIES) {
		float lower_bound = 0.0;
		float upper_bound = 700.0;
		std::uniform_real_distribution<float> unif(lower_bound, upper_bound);
		std::default_random_engine re;


		for (int i = 0; i < numAsteroids; i++) {

			Body* newAsteroid = new Body();

			sf::Vector3f asteroidPos(
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_X)), 
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_Y)), 
				static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / RANDOM_BODY_MAX_Z))
			);

			newAsteroid->Init(asteroidPos, sf::Vector3f(0.0f, 0.0f, 0.0f), RANDOM_BODY_MASS);
			//newAsteroid->Init(sf::Vector2<float>(unif(re), unif(re)), sf::Vector2<float>(0.0, 0.0), 10.0f);
			//newAsteroid->SetColour(sf::Color::Blue);
			bodies_.push_back(newAsteroid);
		}

	}
	bodyCount_ = numAsteroids;
	
	
	if (ADD_ORBIT_BODIES) {

		Body* planet = new Body();
		planet->Init(sf::Vector3f(500.0f, 500.0f, 0.0f), sf::Vector3f(0.0f, 0.0f, 0.0f), 1.0E17f);
		//planet->SetColour(sf::Color::Red);
		bodies_.push_back(planet);

		Body* satelite = new Body();
		satelite->Init(sf::Vector3f(600.0f, 500.0f, 0.0f), sf::Vector3f(0.0f, 25.83215f, 0.0f), 10);
		//satelite->SetColour(sf::Color::Cyan);
		bodies_.push_back(satelite);

		Body* satelite2 = new Body();
		satelite2->Init(sf::Vector3f(400.0f, 500.0f, 0.0f), sf::Vector3f(0.0f, -25.83215f, 0.0f), 10);
		//satelite2->SetColour(sf::Color::Cyan);
		bodies_.push_back(satelite2);

		Body* satelite3 = new Body();
		satelite3->Init(sf::Vector3f(500.0f, 600.0f, 0.0f), sf::Vector3f(-25.83215f, 0.0f, 0.0f), 10);
		//satelite3->SetColour(sf::Color::Cyan);
		bodies_.push_back(satelite3);

		Body* satelite4 = new Body();
		satelite4->Init(sf::Vector3f(500.0f, 400.0f, 0.0f), sf::Vector3f(25.83215f, 0.0f, 0.0f), 10);
		//satelite4->SetColour(sf::Color::Cyan);
		bodies_.push_back(satelite4);

	}
	//bodyCount_ = 2;

	/*
	Body* asteroid1 = new Body();
	asteroid1->Init(sf::Vector2f(100.0f, 100.0f), sf::Vector2f(0.0f, 0.0f), 20.0f);
	bodies_.push_back(asteroid1);

	Body* asteroid2 = new Body();
	asteroid2->Init(sf::Vector2f(1000.0f, 100.0f), sf::Vector2f(0.0f, 0.0f), 20.0f);
	bodies_.push_back(asteroid2);

	*/

	return true;
}


void Simulation::AddBody(Body* newBody) {

	//newBody->SetColour(sf::Color::Green);

	bodies_.push_back(newBody);
}


bool Simulation::Reset() {


	CleanUpBodies();


	if (!GenerateAsteroids(bodyCount_)) {

		return false;
	}


	return true;
}


void Simulation::TimeStep(float dt) {



}


void Simulation::Render(Renderer* renderer, float alpha) {


	for (auto body : bodies_) {

		body->Draw(renderer, alpha);
		//renderer->Draw(*body->Sprite());

	}

}


