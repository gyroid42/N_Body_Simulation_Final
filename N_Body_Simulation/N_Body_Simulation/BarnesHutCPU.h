#pragma once
#include "BarnesHut.h"
#include "Partition.h"
#include "PartitionTree.h"

class BarnesHutCPU :
	public BarnesHut
{
public:
	BarnesHutCPU();
	~BarnesHutCPU();

	void Init();
	void CleanUp();

	void TimeStep(float dt);

protected:

	void PartitionSpace();
	void CalculateForceOnBody(Body* body);

	Partition origin_;
};

