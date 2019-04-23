#pragma once


// include std library classes
#include <vector>

// include libxl
#include <libxl.h>

// include my classes
#include "SimulationSettings.h"

// forward declarations
class Simulation;
class Input;


class BarnesHutAccuracyTester
{
public:
	BarnesHutAccuracyTester();
	~BarnesHutAccuracyTester();


	void Init(Input* newInput);

	void MainLoop();

protected:

	void CreateSimulationSettings();

	void CheckInput();

	void StartSimulation(SimulationSettings* settings);
	void EndSimulation();

	Input* input_;

	Simulation* simulation_;

	std::vector<SimulationSettings> simSettings_;

	libxl::Book* outputBook_;
};

