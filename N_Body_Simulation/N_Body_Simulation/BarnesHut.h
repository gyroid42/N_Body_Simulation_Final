#pragma once
#include "Simulation.h"
#include <SFML\System\Vector3.hpp>
#include <vector>
#include "Partition.h"
class BarnesHut :
	public Simulation
{

public:
	BarnesHut();
	~BarnesHut();

	void Init();
	void CleanUp();

protected:


	void PartitionSpace();
	void CalculateForceOnBody(Body* body);

	Partition origin_;
};

