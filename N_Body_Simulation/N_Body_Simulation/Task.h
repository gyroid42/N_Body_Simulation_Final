#pragma once



// base class for all tasks the farm workers use
class Task
{
public:
	Task();
	virtual ~Task();

	virtual void Run();
};

