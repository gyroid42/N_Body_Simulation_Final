#pragma once

// include parent class
#include "Task.h"


// standard library includes
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

	void Init(std::vector<Body*>* newBodyArray, OctreeNode* newRoot);

	void Run();

private:

	// references to body and partition tree
	//Body* body_;
	std::vector<Body*>* bodyArray_;
	OctreeNode* root_;

};

