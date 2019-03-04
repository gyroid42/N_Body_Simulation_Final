#pragma once
#include "Task.h"

#include "SETTINGS.h"
#include "Channel.h"

class Body;
class OctreeNode;
struct CollisionEvent;


/*

test collision with all bodies on top level before checking lower tasks
probs make it as its own task

*/


class TaskCollisionCheckTree :
	public Task
{
public:
	TaskCollisionCheckTree();
	~TaskCollisionCheckTree();

	void Init(OctreeNode* newRoot, Channel<CollisionEvent*>* newCollisionEventChannel, Body* newAncestorList[MAX_COLLISION_DEPTH], int newDepth);

	void Run();

private:

	OctreeNode* root_;

	Body* ancestorList_[MAX_COLLISION_DEPTH];

	int depth_;


	Channel<CollisionEvent*>* collisionEventsChannel_;


};

