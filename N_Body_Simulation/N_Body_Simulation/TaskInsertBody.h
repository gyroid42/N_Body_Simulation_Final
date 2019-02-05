#pragma once

// include parent class
#include "Task.h"

// forward declaration
class PartitionTree;
class Body;

class TaskInsertBody :
	public Task
{
public:
	TaskInsertBody();
	~TaskInsertBody();

	void Init(Body* newBody, PartitionTree* newRoot);

	void Run();

private:

	// references to body and partition tree
	Body* body_;
	PartitionTree* root_;
};

