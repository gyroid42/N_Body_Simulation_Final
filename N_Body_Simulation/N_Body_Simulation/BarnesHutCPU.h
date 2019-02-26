#pragma once

// include parent class
#include "BarnesHut.h"

// my class includes
#include "ThreadFarm.h"
#include "Channel.h"
#include "Body.h"
#include "BodyChannelData.h"
#include "OctreeCollision.h"

class OctreeNode;

class BarnesHutCPU :
	public BarnesHut
{
public:
	BarnesHutCPU();
	~BarnesHutCPU();

	void Init();
	void CleanUp();

	// overrides time step from Simulation
	// called each physics step
	void TimeStep(float dt);

protected:

	// override PartitionSpace and CalculateForceOnBody from BarnesHut
	void PartitionSpace();
	void CalculateForceOnBody(Body* body);

	// TimeStep methods depending on whether mult-threading is being used
	void TimeStepMultiImproved(float dt);
	void TimeStepMulti(float dt);
	void TimeStepSingle(float dt);

	// farm for managing threads and providing tasks
	ThreadFarm* farm_;


	Channel<OctreeNode*> mergeTreeChannel_;

	Channel<BodyChannelData*> bodyChannels_[8];

	OctreeCollision collisionTree_;

	// function pointer to which time step is being used
	void (BarnesHutCPU::*timeStepFunc_)(float);
};

