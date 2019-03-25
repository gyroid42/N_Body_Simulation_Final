#pragma once

// standard library includes
#include <vector>

// my class includes
#include "SETTINGS.h"
#include "SimulationSettings.h"

// forward declarations
class Body;
class Renderer;
class Application;

class Simulation
{
public:
	Simulation();
	virtual ~Simulation();


	virtual void Init();
	virtual void CleanUp();

	// Generates bodies for the simulation
	virtual bool GenerateSimulation(SIMULATION_MODE simMode);

	// Resets the simulation
	virtual bool Reset(SIMULATION_MODE simMode);

	// TimeStep called each physics step
	virtual void TimeStep(float dt);

	// Render used to draw simulation to render target each frame
	virtual void Render(float alpha);

	// Method to add a new body to the simulation
	virtual void AddBody(Body* newBody);

	inline SimulationSettings* Settings() { return &settings_; }
	inline void NewSettings(SimulationSettings newSettings) { settings_ = newSettings; }

	inline int BodyCount() { return bodies_.size(); }

protected:

	// Cleans up all the resources the bodies use
	void CleanUpBodies();

	// Simulation Mode generators
	bool GenerateRandom();

	// number of bodies currently in the simulation
	int bodyCount_;

	// list of all bodies in the simulation
	std::vector<Body*> bodies_;

	SimulationSettings settings_;
};

