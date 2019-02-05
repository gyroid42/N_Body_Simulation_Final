#pragma once

// include parent class
#include "BarnesHut.h"

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
};

