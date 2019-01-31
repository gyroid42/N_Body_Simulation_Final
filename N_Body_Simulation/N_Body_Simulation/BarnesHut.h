#pragma once
#include "Simulation.h"
#include <SFML\System\Vector3.hpp>
#include <vector>
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

	Partition nodeOrigin_;
};

