
#include "TaskCollisionCheckNode.h"

#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckNode::TaskCollisionCheckNode()
{
}
TaskCollisionCheckNode::~TaskCollisionCheckNode()
{
}


#if BENCHMARKING

void TaskCollisionCheckNode::Init(OctreeNode* newNode, Channel<int>* newCheckCountChannel, Channel<CollisionEvent*>* newCollisionEventsChannel, Body* newAncestorList_[MAX_COLLISION_DEPTH]) {

	node_ = newNode;
	std::memcpy(ancestorList_, newAncestorList_, MAX_COLLISION_DEPTH * sizeof(Body*));
	collisionEventsChannel_ = newCollisionEventsChannel;
	checkCountChannel_ = newCheckCountChannel;
}


void TaskCollisionCheckNode::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	int totalChecks = 0;

	// check collisions
	node_->CheckCollisionSingleNode(ancestorList_, collisionEvents, totalChecks);

	// output collision events found to events channel
	collisionEventsChannel_->write(collisionEvents);

	checkCountChannel_->write(totalChecks);
}


#else
void TaskCollisionCheckNode::Init(OctreeNode* newNode, Channel<CollisionEvent*>* newCollisionEventsChannel,  Body* newAncestorList_[MAX_COLLISION_DEPTH]) {

	node_ = newNode;
	std::memcpy(ancestorList_, newAncestorList_, MAX_COLLISION_DEPTH * sizeof(Body*));
	collisionEventsChannel_ = newCollisionEventsChannel;
}


void TaskCollisionCheckNode::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	// check collisions
	node_->CheckCollisionSingleNode(ancestorList_, collisionEvents);

	// output collision events found to events channel
	collisionEventsChannel_->write(collisionEvents);
}
#endif
