#include "TaskCollisionCheckTree.h"


#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckTree::TaskCollisionCheckTree()
{
}


TaskCollisionCheckTree::~TaskCollisionCheckTree()
{
}


void TaskCollisionCheckTree::Init(OctreeNode* newRoot, Body* newAncestorList[MAX_COLLISION_DEPTH], int newDepth) {

	root_ = newRoot;
	std::memcpy(ancestorList_, newAncestorList, MAX_COLLISION_DEPTH * sizeof(Body*));
	depth_ = newDepth;
}


void TaskCollisionCheckTree::Run() {

	root_->CheckAllCollision(ancestorList_, depth_);
}

