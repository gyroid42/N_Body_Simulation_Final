#pragma once
#include "Task.h"

class Body;

class TaskIntegrateBody :
	public Task
{
public:
	TaskIntegrateBody();
	~TaskIntegrateBody();


	void Init(Body* newBody, float deltaTime);

	void Run();

private:

	// reference to body task is integrating and delta time for this step
	Body* body_;
	float dt_;
};

