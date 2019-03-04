#include "TaskCollisionCheckNode.h"

#include "Body.h"
#include "OctreeNode.h"

TaskCollisionCheckNode::TaskCollisionCheckNode()
{
}


TaskCollisionCheckNode::~TaskCollisionCheckNode()
{
}

void TaskCollisionCheckNode::Init(OctreeNode* newNode, Channel<CollisionEvent*>* newCollisionEventsChannel,  Body* newComparisonList[MAX_COLLISION_DEPTH]) {

	node_ = newNode;
	std::memcpy(comparisonList_, newComparisonList, MAX_COLLISION_DEPTH * sizeof(Body*));
	collisionEventsChannel_ = newCollisionEventsChannel;
}


void TaskCollisionCheckNode::Run() {

	CollisionEvent* collisionEvents = nullptr;

	node_->CheckCollisionSingleNode(comparisonList_, collisionEvents, 1);

	collisionEventsChannel_->write(collisionEvents);
}
