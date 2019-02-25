#pragma once

// include parent class
#include "Task.h"

#include <vector>


// forward declaration
class OctreeNode;
class Body;

class TaskUpdateForces :
	public Task
{
public:
	TaskUpdateForces();
	~TaskUpdateForces();

	void Init(OctreeNode* globalRoot, std::vector<Body*>* newBodyArray);

	void Run();

private:

	// references to body and partition tree
	std::vector<Body*>* bodyArray_;
	OctreeNode* localRoot_;
};

