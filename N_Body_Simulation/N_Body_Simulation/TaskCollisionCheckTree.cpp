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

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	// check collisions
	root_->CheckAllCollision(ancestorList_, collisionEvents, depth_);

	// output collision events found to events channel
	collisionEventsChannel_->write(collisionEvents);
}

