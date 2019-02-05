
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


void TaskBruteForceCPU::Init(Body* newBody, BruteForce* newSimulation) {

	// Set references to body and simulation
	body_ = newBody;
	simulation_ = newSimulation;
}


void TaskBruteForceCPU::Run() {

	// run method for calculating force on body
	simulation_->CalculateForceOnBody(body_);
}

