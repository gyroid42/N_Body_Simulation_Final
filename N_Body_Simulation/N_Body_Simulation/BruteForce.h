#pragma once

#include <list>
#include "Body.h"
#include "PhysicsUtil.h"
#include "Simulation.h"
#include "ThreadFarm.h"

class BruteForce :
	public Simulation
{
public:
	BruteForce();
	~BruteForce();

	void Init();

	void CleanUp();

	void TimeStep(float dt);

	void CalculateForceOnBody(Body* body);

private:


	void TimeStepMulti(float dt);
	void TimeStepSingle(float dt);


	ThreadFarm* farm_;

	void (BruteForce::*timeStepFunc_)(float);
};

