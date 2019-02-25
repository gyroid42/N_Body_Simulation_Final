#pragma once

// include parent class
#include "Task.h"

#include "Channel.h"
#include "Partition.h"
#include <vector>



// forward declaration
class OctreeNode;
class Body;


class TaskInsertBody :
	public Task
{
public:
	TaskInsertBody();
	~TaskInsertBody();

	void Init(Channel<OctreeNode*>* newChannel, OctreeNode* newRoot, std::vector<Body*>* newBodyArray);

	void Run();

private:

	// references to body and partition tree
	std::vector<Body*>* bodyArray_;
	OctreeNode* root_;
	Channel<OctreeNode*>* outputChannel_;
};

