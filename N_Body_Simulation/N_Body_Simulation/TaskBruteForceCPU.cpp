
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

