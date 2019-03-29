
// class header include
#include "TaskUpdateForces.h"

// my class includes
#include "OctreeNode.h"
#include "Body.h"


TaskUpdateForces::TaskUpdateForces()
{
}


TaskUpdateForces::~TaskUpdateForces()
{
}


void TaskUpdateForces::Init(std::vector<Body*>* newBodyArray, OctreeNode* newRoot, float newTheta) {

	// set references to body and partition tree
	bodyArray_ = newBodyArray;
	root_ = newRoot;
	theta_ = newTheta;
}

void TaskUpdateForces::Run() {

	for (auto body : *bodyArray_) {
		// run method for calculating force on body

		body->ResetForce();
		root_->UpdateForceOnBody(body, theta_);
	}
}
