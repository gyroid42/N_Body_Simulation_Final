#pragma once

// include parent class
#include "Simulation.h"

// standard library includes
#include <list>

// my class includes
#include "ThreadFarm.h"
#include "OctreeCollision.h"

struct CollisionEvent;

class BruteForce :
	public Simulation
{
public:
	BruteForce();
	~BruteForce();

	void Init();
	void CleanUp();


	// Overrides TimeStep from Simulation
	// called on each physics step
	void TimeStep(float dt);


	// calculates the force acting on a body
	void CalculateForceOnBody(Body* body);
	void CalculateForceOnBody(Body* body, int& totalForceCalcs);


	void CheckAllCollisions(std::vector<std::vector<Body*>*>* bodyArrays);
	void CheckCollision(Body* body, CollisionEvent*& collisionEvents);
	void CheckCollision(Body* body, CollisionEvent*& collisionEvents, int& totalChecks);


private:

	bool SphereToSphereCollision(Body* b1, Body* b2);


	// TimeStep methods depending on whether multithreading is being used
	void TimeStepMulti(float dt);
	void TimeStepSingle(float dt);

	// farm for managing threads and providing tasks
	ThreadFarm* farm_;

	OctreeCollision collisionTree_;


	// function pointer to which time step is being used
	void (BruteForce::*timeStepFunc_)(float);
};

