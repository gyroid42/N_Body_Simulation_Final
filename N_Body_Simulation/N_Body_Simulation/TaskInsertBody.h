#pragma once

// include parent class
#include "Task.h"

#include "Channel.h"
#include "Partition.h"
#include <vector>



// forward declaration
class PartitionTree;
class Body;


class TaskInsertBody :
	public Task
{
public:
	TaskInsertBody();
	~TaskInsertBody();

	void Init(Channel<PartitionTree*>* newChannel, Partition newPartition, std::vector<Body*>* newBodyArray);

	void Run();

private:

	// references to body and partition tree
	std::vector<Body*>* bodyArray_;
	PartitionTree* root_;
	Channel<PartitionTree*>* outputChannel_;
};

