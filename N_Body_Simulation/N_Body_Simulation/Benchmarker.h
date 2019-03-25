#pragma once


// Include std library classes
#include <vector>

// Include libxl
#include <libxl.h>

// Include my classes
#include "SimulationSettings.h"

class Simulation;
class Input;


class Benchmarker
{
public:
	Benchmarker();
	~Benchmarker();

	void Init(Input* newInput);

	void MainLoop();


protected:

	void CreateSimulationSettings();


	void CheckInput();



	void StartSimulation(SimulationSettings* settings);
	void EndSimulation();

	Input* input_;

	Simulation* simulation_;

	std::vector<SimulationSettings> benchmarkSettingsList_;

	libxl::Book* outputBook_;
};

