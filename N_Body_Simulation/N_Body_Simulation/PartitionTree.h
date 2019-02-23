#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <mutex>


// my class includes
#include "Partition.h"

// forward declarations
class Body;

class PartitionTree
{
public:
	PartitionTree(Partition newPartition);
	~PartitionTree();

	// inserts a new body into the partition tree
	void Insert(Body* body);

	void InsertMulti(Body* body);

	// Update the force on a given body by traversing the tree
	void UpdateForceOnBody(Body* body);



	void Merge(PartitionTree* mergeTree);

	void AddBody(Body* body);

	void CreateChildren();

	PartitionTree* GetChild(int index);
	inline void SetChild(int index, PartitionTree* newChild) { children_[index] = newChild; }

	inline float Mass() { return totalMass_; }
	inline sf::Vector3f CenterOfMass() { return centerOfMass_; }
	inline Body* GetBody() { return body_; }
	inline bool IsExternal() { return isExternal_; }

private:

	// Partitioned space this node contains
	Partition partition_;

	// total mass and center of mass within this partition
	float totalMass_;
	sf::Vector3f centerOfMass_;

	// reference to all the octants this partition contains
	PartitionTree* children_[8];

	// reference to initial body inserted in this partition
	Body* body_;

	// isExternal is true if this is this partition only contains 1 body
	bool isExternal_;

	
	std::mutex totalMassMutex_;
	std::mutex isExternalMutex_;
	std::mutex childrenMutex_[8];


	int numBodies_;
};

