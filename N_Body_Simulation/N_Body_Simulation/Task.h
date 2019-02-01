#pragma once
class Task
{
public:
	Task();
	virtual ~Task() = 0;


	virtual void Run() = 0;
};

