#pragma once

// include parent class
#include "Simulation.h"

// standard library includes
#include <list>

// my class includes
#include "ThreadFarm.h"

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

private:


	// TimeStep methods depending on whether multithreading is being used
	void TimeStepMulti(float dt);
	void TimeStepSingle(float dt);

	// farm for managing threads and providing tasks
	ThreadFarm* farm_;

	// function pointer to which time step is being used
	void (BruteForce::*timeStepFunc_)(float);
};

