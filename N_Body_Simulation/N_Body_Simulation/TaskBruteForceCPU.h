#pragma once

// include parent class
#include "Task.h"

// forward declarations
class BruteForce;
class Body;


class TaskBruteForceCPU :
	public Task
{
public:
	TaskBruteForceCPU();
	~TaskBruteForceCPU();

	void Init(Body* newBody, BruteForce* newSimulation);

	void Run();

private:

	// references to body and simulation
	Body* body_;
	BruteForce* simulation_;
};

