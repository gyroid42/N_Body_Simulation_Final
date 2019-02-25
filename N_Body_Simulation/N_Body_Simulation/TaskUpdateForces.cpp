
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


void TaskUpdateForces::Init(OctreeNode* globalRoot, std::vector<Body*>* newBodyArray) {

	// set references to body and partition tree
	bodyArray_ = newBodyArray;
	localRoot_ = globalRoot->GetCopy();
	localRoot_->Localise();
}

void TaskUpdateForces::Run() {

	for (auto body : *bodyArray_) {

		// run method for calculating force on body
		body->ResetForce();

		localRoot_->UpdateForceOnBody(body);
	}

	// delete body array and local tree
	//if (bodyArray_) {

		//bodyArray_->clear();
		//delete bodyArray_;
		//bodyArray_ = nullptr;
	//}

	if (localRoot_) {

		delete localRoot_;
		localRoot_ = nullptr;
	}
}
