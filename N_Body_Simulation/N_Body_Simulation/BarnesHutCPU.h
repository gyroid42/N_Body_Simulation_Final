#pragma once
#include "BarnesHut.h"

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
};

