#pragma once

// standard library includes
#include <vector>
#include <mutex>

// my class includes
#include "SETTINGS.h"
#include "SimulationSettings.h"
#include "Model.h"

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
	virtual bool Reset();

	// TimeStep called each physics step
	virtual void TimeStep(float dt);

	// Render used to draw simulation to render target each frame
	virtual void Render(float alpha);

	// Method to add a new body to the simulation
	virtual void AddBody(Body* newBody);

	inline SimulationSettings* Settings() { return &settings_; }
	inline void NewSettings(SimulationSettings newSettings) { settings_ = newSettings; }

	inline int BodyCount() { return bodies_.size(); }
	inline float RenderTime() { return renderTime_; }
	inline float InsertTime() { return insertTime_; }
	inline float ForceTime() { return forceTime_; }
	inline float IntegrationTime() { return integrationTime_; }
	inline float SortTime() { return sortTime_; }
	inline float CollisionTime() { return collisionTime_; }


	Body* GetBody(std::string bodyName);

#if BENCHMARKING

	inline std::vector<int> GetForceCalcTimes() { return forceCalcTimes_; }
	inline std::vector<int> GetCollisionCheckTimes() { return collisionCheckTimes_; }
	inline std::vector<int> GetIntegrationTimes() { return integrationTimes_; }

	inline std::vector<int> GetNumForceCalcs() { return numForceCalculations_; }
	inline std::vector<int> GetNumCollisionChecks() { return numCollisionChecks_; }

#endif

	void ShiftBodyStates();


protected:

	// Cleans up all the resources the bodies use
	void CleanUpBodies();

	// Simulation Mode generators
	bool GenerateRandom();
	bool GenerateGrid();
	bool GenerateClusters();
	bool GenerateAsteroidBelt();
	bool GenerateTwoBodyOrbit();



	// number of bodies currently in the simulation
	int bodyCount_;

	// computation times for each step
	float insertTime_, forceTime_, integrationTime_, sortTime_, collisionTime_, renderTime_;

	// list of all bodies in the simulation
	std::vector<Body*> bodies_;
	std::mutex bodyListMutex_;

	SimulationSettings settings_;


	Model bodyModel_;

#if BENCHMARKING

	std::vector<int> forceCalcTimes_;
	std::vector<int> collisionCheckTimes_;
	std::vector<int> integrationTimes_;

	std::vector<int> numForceCalculations_;
	std::vector<int> numCollisionChecks_;

#endif

};

