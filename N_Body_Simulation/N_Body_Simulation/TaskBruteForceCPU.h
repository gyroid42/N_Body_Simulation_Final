#pragma once
#include "Task.h"

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

	Body* body_;
	BruteForce* simulation_;
};

