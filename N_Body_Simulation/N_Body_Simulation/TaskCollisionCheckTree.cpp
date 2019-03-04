#include "TaskCollisionCheckTree.h"


#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckTree::TaskCollisionCheckTree()
{
}


TaskCollisionCheckTree::~TaskCollisionCheckTree()
{
}


void TaskCollisionCheckTree::Init(OctreeNode* newRoot, Channel<CollisionEvent*>* newCollisionEventChannel, Body* newAncestorList[MAX_COLLISION_DEPTH], int newDepth) {

	root_ = newRoot;
	std::memcpy(ancestorList_, newAncestorList, MAX_COLLISION_DEPTH * sizeof(Body*));
	depth_ = newDepth;
	collisionEventsChannel_ = newCollisionEventChannel;
}


void TaskCollisionCheckTree::Run() {

	CollisionEvent* collisionEvents = nullptr;

	root_->CheckAllCollision(ancestorList_, collisionEvents, depth_);

	collisionEventsChannel_->write(collisionEvents);
}

