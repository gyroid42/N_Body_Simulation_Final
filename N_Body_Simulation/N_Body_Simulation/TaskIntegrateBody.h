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

	Body* body_;
	float dt_;
};

