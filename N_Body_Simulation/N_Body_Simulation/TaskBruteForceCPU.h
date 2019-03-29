#pragma once

// include parent class
#include "Task.h"

// standard library includes
#include <vector>

// forward declarations
class BruteForce;
class Body;


class TaskBruteForceCPU :
	public Task
{
public:
	TaskBruteForceCPU();
	~TaskBruteForceCPU();

	void Init(std::vector<Body*>* newBodyArray, BruteForce* newSimulation);

	void Run();

private:

	// references to body and simulation
	std::vector<Body*>* bodyArray_;
	BruteForce* simulation_;
};

