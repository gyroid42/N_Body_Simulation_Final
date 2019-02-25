#pragma once

// include parent class
#include "Task.h"


// forward declaration
class OctreeNode;
class Body;

class TaskUpdateForces :
	public Task
{
public:
	TaskUpdateForces();
	~TaskUpdateForces();

	void Init(Body* newBody, OctreeNode* newRoot);

	void Run();

private:

	// references to body and partition tree
	Body* body_;
	OctreeNode* root_;

};

