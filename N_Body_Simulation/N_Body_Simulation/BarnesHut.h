#pragma once

// include parent class
#include "Simulation.h"

// SFML includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <vector>

// my class includes
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

	// Partition Space is a virtual function which is used to create the partition tree of the physics space
	virtual void PartitionSpace();

	// Calculates the force on a body due to another body
	virtual void CalculateForceOnBody(Body* body);

	// Root of partition space tree
	Partition root_;
};

