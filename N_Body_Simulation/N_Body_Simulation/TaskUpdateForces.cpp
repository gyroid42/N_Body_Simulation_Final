
// class header include
#include "TaskUpdateForces.h"

// my class includes
#include "PartitionTree.h"
#include "Body.h"


TaskUpdateForces::TaskUpdateForces()
{
}


TaskUpdateForces::~TaskUpdateForces()
{
}


void TaskUpdateForces::Init(Body* newBody, PartitionTree* newRoot) {

	// set references to body and partition tree
	body_ = newBody;
	root_ = newRoot;
}

void TaskUpdateForces::Run() {

	// run method for calculating force on body
	body_->ResetForce();
	root_->UpdateForceOnBody(body_);
}
