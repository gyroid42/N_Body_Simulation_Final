#include "TaskCollisionCheckNode.h"

#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckNode::TaskCollisionCheckNode()
{
}


TaskCollisionCheckNode::~TaskCollisionCheckNode()
{
}

void TaskCollisionCheckNode::Init(OctreeNode* newNode, Body* newComparisonList[MAX_COLLISION_DEPTH]) {

	node_ = newNode;
	std::memcpy(comparisonList_, newComparisonList, MAX_COLLISION_DEPTH * sizeof(Body*));
}


void TaskCollisionCheckNode::Run() {

	node_->CheckCollisionSingleNode(comparisonList_);
}
