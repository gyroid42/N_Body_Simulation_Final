
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

void TaskInsertBody::Init(Body* newBody, PartitionTree* newRoot) {

	// set references to body and partition tree
	body_ = newBody;
	root_ = newRoot;
}

void TaskInsertBody::Run() {

	// run method for adding body into partition tree
	root_->InsertMulti(body_);
}
