
// class header include
#include "TaskBruteForceCPU.h"


// my class includes
#include "BruteForce.h"
#include "Body.h"


TaskBruteForceCPU::TaskBruteForceCPU()
{
}


TaskBruteForceCPU::~TaskBruteForceCPU()
{
}

#if BENCHMARKING
void TaskBruteForceCPU::Init(Channel<int>* newCalcCountChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation) {

	// Set references to body and simulation
	bodyArray_ = newBodyArray;
	simulation_ = newSimulation;
	calcCountChannel_ = newCalcCountChannel;
}


void TaskBruteForceCPU::Run() {

	int totalForceCalcs = 0;

	for (auto body : *bodyArray_) {

		// run method for calculating force on body
		simulation_->CalculateForceOnBody(body, totalForceCalcs);
	}

	calcCountChannel_->write(totalForceCalcs);
}

#else
void TaskBruteForceCPU::Init(std::vector<Body*>* newBodyArray, BruteForce* newSimulation) {

	// Set references to body and simulation
	bodyArray_ = newBodyArray;
	simulation_ = newSimulation;
}


void TaskBruteForceCPU::Run() {

	for (auto body : *bodyArray_) {

		// run method for calculating force on body
		simulation_->CalculateForceOnBody(body);
	}
}
#endif




