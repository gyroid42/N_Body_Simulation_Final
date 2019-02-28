#pragma once

// standard library includes
#include <vector>

// my class includes
#include "SETTINGS.h"

// forward declarations
class Body;
class Renderer;


class Simulation
{
public:
	Simulation();
	virtual ~Simulation();


	virtual void Init();
	virtual void CleanUp();

	// Generates bodies for the simulation
	virtual bool GenerateAsteroids(int numAsteroids);

	// Resets the simulation
	virtual bool Reset();

	// TimeStep called each physics step
	virtual void TimeStep(float dt);

	// Render used to draw simulation to render target each frame
	virtual void Render(float alpha);

	// Method to add a new body to the simulation
	virtual void AddBody(Body* newBody);

protected:

	// Cleans up all the resources the bodies use
	void CleanUpBodies();

	// number of bodies currently in the simulation
	int bodyCount_;

	// list of all bodies in the simulation
	std::vector<Body*> bodies_;
};

