#include "Benchmarker.h"


// std library includes
#include <iostream>

// My class includes
#include "Simulation.h"
#include "Input.h"
#include "BarnesHutCPU.h"
#include "BruteForce.h"


Benchmarker::Benchmarker() :
	input_(nullptr),
	simulation_(nullptr),
	outputBook_(nullptr)
{
}


Benchmarker::~Benchmarker()
{

	if (input_) {

		delete input_;
		input_ = nullptr;
	}

	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}

	if (outputBook_) {

		outputBook_->release();
		outputBook_ = nullptr;
	}
}





void Benchmarker::Init(Input* newInput) {

	input_ = newInput;

	outputBook_ = xlCreateBook();

	CreateSimulationSettings();
}


void Benchmarker::CreateSimulationSettings() {

	
}



void Benchmarker::MainLoop() {


	for (auto currentSettings : benchmarkSettingsList_) {

		StartSimulation(&currentSettings);


		for (int i = 0; i < currentSettings.maxSteps; i++) {

			simulation_->TimeStep(currentSettings.dt);
		}


		// write results into excel spreadsheet
	}

}


void Benchmarker::CheckInput() {


}



void Benchmarker::StartSimulation(SimulationSettings* settings) {

	if (simulation_) {

		EndSimulation();
	}

	switch (settings->simMethod) {
	case Direct:
		simulation_ = new BruteForce();
		break;
	case Barnes_Hut:
		simulation_ = new BarnesHutCPU();
		break;
	default:
		simulation_ = new BruteForce();
		break;
	}

	simulation_->Init(settings);
	simulation_->Reset(settings->simMode);
}


void Benchmarker::EndSimulation() {

}
