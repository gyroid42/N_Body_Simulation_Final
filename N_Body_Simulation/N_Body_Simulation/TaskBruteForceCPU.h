#pragma once

// include parent class
#include "Task.h"

// standard library includes
#include <vector>

// include my classes
#include "Channel.h"
#include "SETTINGS.h"

// forward declarations
class BruteForce;
class Body;


class TaskBruteForceCPU :
	public Task
{
public:
	TaskBruteForceCPU();
	~TaskBruteForceCPU();

#if BENCHMARKING
	void Init(Channel<int>* newCalcCountChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation);

#else
	void Init(std::vector<Body*>* newBodyArray, BruteForce* newSimulation);
#endif
	void Run();

private:

	// references to body and simulation
	std::vector<Body*>* bodyArray_;
	BruteForce* simulation_;

#if BENCHMARKING
	Channel<int>* calcCountChannel_;
#endif
};

