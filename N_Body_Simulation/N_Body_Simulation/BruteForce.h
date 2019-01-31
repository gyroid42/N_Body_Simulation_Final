#pragma once

#include <list>
#include "Body.h"
#include "PhysicsUtil.h"
#include "Simulation.h"

class BruteForce :
	public Simulation
{
public:
	BruteForce();
	~BruteForce();

	void TimeStep(float dt);

};

