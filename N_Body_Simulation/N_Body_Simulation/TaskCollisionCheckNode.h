#pragma once
#include "Task.h"


#include "SETTINGS.h"
#include "Channel.h"

class Body;
class OctreeNode;
struct CollisionEvent;

class TaskCollisionCheckNode :
	public Task
{
public:
	TaskCollisionCheckNode();
	~TaskCollisionCheckNode();

	void Init(OctreeNode* newNode, Channel<CollisionEvent*>* newCollisionEventsChannel, Body* newComparisonList[MAX_COLLISION_DEPTH]);

	void Run();

private:
	
	OctreeNode* node_;

	Body* comparisonList_[MAX_COLLISION_DEPTH];

	Channel<CollisionEvent*>* collisionEventsChannel_;
};

