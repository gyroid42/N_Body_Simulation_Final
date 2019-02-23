
// class header include
#include "TaskInsertBody.h"

// my class includes
#include "PartitionTree.h"
#include "Body.h"


TaskInsertBody::TaskInsertBody()
{
}


TaskInsertBody::~TaskInsertBody()
{
}

void TaskInsertBody::Init(Channel<PartitionTree*>* newChannel, Partition newPartition, std::vector<Body*>* newBodyArray) {

	// set references to body and partition tree
	bodyArray_ = newBodyArray;
	outputChannel_ = newChannel;
	root_ = new PartitionTree(newPartition);
}

void TaskInsertBody::Run() {

	for (auto body : *bodyArray_) {

		root_->Insert(body);
	}


	outputChannel_->write(root_);
}
