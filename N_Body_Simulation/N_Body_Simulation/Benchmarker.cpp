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

	EndSimulation();

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


	// run a simulation for each setting
	for (auto currentSettings : benchmarkSettingsList_) {

		// loop for each thread count in range (1, 2, 4, 6, 8, 10, 12...)
		for (; currentSettings.threadCount <= std::thread::hardware_concurrency(); (currentSettings.threadCount == 1) ? currentSettings.threadCount++ : currentSettings.threadCount += 2) {

			// loop for each body count in body count list
			for (int bodyCountIndex = 0; bodyCountIndex < 20; bodyCountIndex++) {

				// update body count for current simulation
				currentSettings.bodyCount = currentSettings.bodyCountList[bodyCountIndex];

				
				StartSimulation(&currentSettings);


				// do a timestep until max has been hit
				for (int i = 0; i < currentSettings.maxSteps; i++) {

					simulation_->TimeStep(currentSettings.dt);
				}


				// write results into excel spreadsheet




				if (!currentSettings.varyBodies) {

					break;
				}
			}

			if (!currentSettings.varyMultiThreading) {

				break;
			}

		}
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

	simulation_->NewSettings(*settings);
	simulation_->Init();
	simulation_->Reset();
}


void Benchmarker::EndSimulation() {


	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}
}
