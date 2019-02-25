#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <vector>

// my class includes
#include "Partition.h"

// forward declarations
class Body;


class OctreeNode
{
public:
	OctreeNode(Partition newPartition);
	OctreeNode(Partition newPartition, OctreeNode* newRoot);
	~OctreeNode();

	// inserts a new body into the partition tree
	void Insert(Body* body, int& counter);


	// Update the force on a given body by traversing the tree
	void UpdateForceOnBody(Body* body);


	// Merges two Octrees together
	void Merge(OctreeNode* mergeTree);

	// Gets list of bodies
	void GetOrderedElementsList(std::vector<Body*>& newList_);

	// Adds new body to this node
	void AddBody(Body* body);

	// Creates empty children on this node
	void CreateChildren();

	// Getters
	OctreeNode* GetChild(int index);
	inline float Mass() { return totalMass_; }
	inline sf::Vector3f CenterOfMass() { return centerOfMass_; }
	inline Body* GetBody() { return body_; }
	inline bool IsExternal() { return isExternal_; }
	inline bool IsLocalised() { return isLocalised_; }
	inline int NumBodies() { return numBodies_; }
	inline std::vector<Body*>& GetBodyList() { return bodyList_; }
	inline Partition GetPartition() { return partition_; }
	inline OctreeNode* Root() { return treeRoot_; }
	void Localise();
	OctreeNode* GetCopy();

	// Setters
	inline void SetChild(int index, OctreeNode* newChild) { children_[index] = newChild; }
	inline void SetExternal(bool newValue) { isExternal_ = newValue; }
	inline void SetMass(float newMass) { totalMass_ = newMass; }
	inline void SetCoM(sf::Vector3f newCoM) { centerOfMass_ = newCoM; }
	inline void SetBody(Body* newBody) { body_ = newBody; }
	inline void SetLocalised(bool newValue) { isLocalised_ = newValue; }

private:

	// Partitioned space this node contains
	Partition partition_;

	OctreeNode* treeRoot_;

	// total mass and center of mass within this partition
	float totalMass_;
	sf::Vector3f centerOfMass_;

	// reference to all the octants this partition contains
	OctreeNode* children_[8];

	// reference to initial body inserted in this partition
	Body* body_;

	std::vector<Body*> bodyList_;

	// isExternal is true if this is this partition only contains 1 body
	bool isExternal_;

	bool isLocalised_;

	int numBodies_;
};

