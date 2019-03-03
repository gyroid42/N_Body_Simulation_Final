#pragma once
#include "Task.h"


#include "SETTINGS.h"

class Body;
class OctreeNode;

class TaskCollisionCheckNode :
	public Task
{
public:
	TaskCollisionCheckNode();
	~TaskCollisionCheckNode();

	void Init(OctreeNode* newNode, Body* newComparisonList[MAX_COLLISION_DEPTH]);

	void Run();

private:
	
	OctreeNode* node_;

	Body* comparisonList_[MAX_COLLISION_DEPTH];
};

