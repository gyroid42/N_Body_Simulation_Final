#pragma once
class Task
{
public:
	Task();
	virtual ~Task();


	virtual void run() = 0;
};

