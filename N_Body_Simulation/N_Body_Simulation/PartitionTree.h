#pragma once
#include <SFML\System\Vector3.hpp>
#include "Partition.h"
#include "Body.h"

class Body;

class PartitionTree
{
public:
	PartitionTree(Partition newPartition);
	~PartitionTree();


	void Insert(Body* body);
	void UpdateForceOnBody(Body* body);

private:

	Partition partition_;

	float totalMass_;
	sf::Vector3f centerOfMass_;

	PartitionTree* children_[8];


	Body* body_;

	bool isExternal_;


};

