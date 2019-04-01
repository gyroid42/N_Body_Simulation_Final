#pragma once

// include parent class
#include "Task.h"

// include my classes
#include "SETTINGS.h"
#include "Channel.h"

// forward decleration
class Body;
class OctreeNode;
struct CollisionEvent;


class TaskCollisionCheckNode :
	public Task
{
public:
	TaskCollisionCheckNode();
	~TaskCollisionCheckNode();

#if BENCHMARKING
	void Init(OctreeNode* newNode, Channel<int>* newCheckCountChannel, Channel<CollisionEvent*>* newCollisionEventsChannel, Body* newAncestorList[MAX_COLLISION_DEPTH]);
#else
	void Init(OctreeNode* newNode, Channel<CollisionEvent*>* newCollisionEventsChannel, Body* newAncestorList[MAX_COLLISION_DEPTH]);
#endif

	void Run();

private:
	
	// node being checked
	OctreeNode* node_;

	// list of ancestors to this node
	Body* ancestorList_[MAX_COLLISION_DEPTH];

	// channel to output collisions found
	Channel<CollisionEvent*>* collisionEventsChannel_;

#if BENCHMARKING
	Channel<int>* checkCountChannel_;
#endif
};

