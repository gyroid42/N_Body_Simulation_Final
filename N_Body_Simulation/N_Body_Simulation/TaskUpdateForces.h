#pragma once

// include parent class
#include "Task.h"


// forward declaration
class PartitionTree;
class Body;

class TaskUpdateForces :
	public Task
{
public:
	TaskUpdateForces();
	~TaskUpdateForces();

	void Init(Body* newBody, PartitionTree* newRoot);

	void Run();

private:

	// references to body and partition tree
	Body* body_;
	PartitionTree* root_;

};

