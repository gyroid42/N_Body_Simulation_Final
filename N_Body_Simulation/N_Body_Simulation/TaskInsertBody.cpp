
// class header include
#include "TaskInsertBody.h"

// my class includes
#include "OctreeNode.h"
#include "Body.h"


TaskInsertBody::TaskInsertBody()
{
}


TaskInsertBody::~TaskInsertBody()
{
}

void TaskInsertBody::Init(Channel<OctreeNode*>* newChannel, OctreeNode* newRoot, std::vector<Body*>* newBodyArray) {

	// set references to body and partition tree
	bodyArray_ = newBodyArray;
	outputChannel_ = newChannel;
	root_ = newRoot;
}

void TaskInsertBody::Run() {
	
	for (auto body : *bodyArray_) {
		
		int counter = 0;
		root_->Insert(body, counter);
	}

	if (bodyArray_) {

		bodyArray_->clear();
		delete bodyArray_;
		bodyArray_ = nullptr;
	}

	outputChannel_->write(root_);
}
