#include "TaskCollisionCheckTree.h"


#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckTree::TaskCollisionCheckTree()
{
}


TaskCollisionCheckTree::~TaskCollisionCheckTree()
{
}

#if BENCHMARKING

void TaskCollisionCheckTree::Init(OctreeNode* newRoot, Channel<int>* newCheckCountChannel, Channel<CollisionEvent*>* newCollisionEventChannel, Body* newAncestorList[MAX_COLLISION_DEPTH]) {

	root_ = newRoot;
	std::memcpy(ancestorList_, newAncestorList, MAX_COLLISION_DEPTH * sizeof(Body*));
	collisionEventsChannel_ = newCollisionEventChannel;
	checkCountChannel_ = newCheckCountChannel;
}


void TaskCollisionCheckTree::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	int totalChecks = 0;

	// check collisions
	root_->CheckAllCollision(ancestorList_, collisionEvents, totalChecks);

	// output collision events found to events channel
	collisionEventsChannel_->write(collisionEvents);

	checkCountChannel_->write(totalChecks);
}

#else
void TaskCollisionCheckTree::Init(OctreeNode* newRoot, Channel<CollisionEvent*>* newCollisionEventChannel, Body* newAncestorList[MAX_COLLISION_DEPTH]) {

	root_ = newRoot;
	std::memcpy(ancestorList_, newAncestorList, MAX_COLLISION_DEPTH * sizeof(Body*));
	collisionEventsChannel_ = newCollisionEventChannel;
}


void TaskCollisionCheckTree::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	// check collisions
	root_->CheckAllCollision(ancestorList_, collisionEvents);

	// output collision events found to events channel
	collisionEventsChannel_->write(collisionEvents);
}


#endif
