#pragma once
#include "Task.h"


#include <vector>

// forward declarations
class Body;

class TaskIntegration :
	public Task
{
public:
	TaskIntegration();
	~TaskIntegration();


	void Init(std::vector<Body*>* newBodyArray, float newDelta);

	void Run();

private:

	std::vector<Body*>* bodyArray_;
	float dt_;
};

