#pragma once

// include parent class
#include "Task.h"

// include my classes
#include "SETTINGS.h"
#include "Channel.h"

// forward declarations
class Body;
class OctreeNode;
struct CollisionEvent;


class TaskCollisionCheckTree :
	public Task
{
public:
	TaskCollisionCheckTree();
	~TaskCollisionCheckTree();

	void Init(OctreeNode* newRoot, Channel<CollisionEvent*>* newCollisionEventChannel, Body* newAncestorList[MAX_COLLISION_DEPTH], int newDepth);

	void Run();

private:

	// root of octree being checked
	OctreeNode* root_;

	// list of ancestors to this node
	Body* ancestorList_[MAX_COLLISION_DEPTH];

	// depth of this node
	int depth_;

	// channel to output collisions found
	Channel<CollisionEvent*>* collisionEventsChannel_;


};

