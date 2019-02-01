#include "TaskBruteForceCPU.h"

#include "BruteForce.h"
#include "Body.h"


TaskBruteForceCPU::TaskBruteForceCPU()
{
}


TaskBruteForceCPU::~TaskBruteForceCPU()
{
}


void TaskBruteForceCPU::Init(Body* newBody, BruteForce* newSimulation) {

	body_ = newBody;
	simulation_ = newSimulation;
}


void TaskBruteForceCPU::Run() {

	simulation_->CalculateForceOnBody(body_);
}

