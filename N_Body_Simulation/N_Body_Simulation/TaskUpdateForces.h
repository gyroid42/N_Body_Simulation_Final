#pragma once

// include parent class
#include "Task.h"

// standard library includes
#include <vector>

// include my classes
#include "Channel.h"
#include "SETTINGS.h"

// forward declaration
class OctreeNode;
class Body;

class TaskUpdateForces :
	public Task
{
public:
	TaskUpdateForces();
	~TaskUpdateForces();

#if BENCHMARKING
	void Init(Channel<int>* newCalcCountChannel, std::vector<Body*>* newBodyArray, OctreeNode* newRoot, float newTheta);
#else
	void Init(std::vector<Body*>* newBodyArray, OctreeNode* newRoot, float newTheta);
#endif

	void Run();

private:

	// references to body and partition tree
	//Body* body_;
	std::vector<Body*>* bodyArray_;
	OctreeNode* root_;

	float theta_;

#if BENCHMARKING
	Channel<int>* calcCountChannel_;
#endif
};

