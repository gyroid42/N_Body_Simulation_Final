#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <vector>

// my class includes
#include "Partition.h"
#include "Channel.h"

// forward declarations
class Body;
class ThreadFarm;

struct CollisionEvent {
	Body* b1;
	Body* b2;
	CollisionEvent* next;

	CollisionEvent(Body* newB1 = nullptr, Body* newB2 = nullptr, CollisionEvent* nextCollision = nullptr) {
		b1 = newB1;
		b2 = newB2;
		next = nextCollision;
	}

	~CollisionEvent() {

		if (next) {

			delete next;
			next = nullptr;
		}
	}
};

class OctreeNode
{
public:
	OctreeNode(Partition newPartition);
	OctreeNode(Partition newPartition, OctreeNode* newRoot, int newDepth);
	~OctreeNode();

	// inserts a new body into the partition tree
	void Insert(Body* body, int& depthCounter);


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

	// Starts collision checks
	void CollisionCheckParallel(ThreadFarm* farm, int bodyNumPerTask);

	// checks collision in all the nodes
	void CheckAllCollision(Body* ancestorList[], CollisionEvent*& collisionEvents, unsigned short int depth);
	void CheckCollisionSingleNode(Body* ancestorList[], CollisionEvent*& collisionEvents, unsigned short int depth);

	// Check Collision between 2 bodies
	bool TestCollision(Body* b1, Body* b2);

	// Getters
	OctreeNode* GetChild(int index);
	inline float Mass() { return totalMass_; }
	inline sf::Vector3f CenterOfMass() { return centerOfMass_; }
	inline Body* GetBody() { return body_; }
	inline Body* GetBodyList() { return bodyList_; }
	inline Body* GetBodyListEnd() { return bodyListEnd_; }
	inline bool IsExternal() { return isExternal_; }
	inline int NumBodies() { return numBodies_; }
	inline Partition GetPartition() { return partition_; }
	inline OctreeNode* Root() { return treeRoot_; }

	// Setters
	inline void SetChild(int index, OctreeNode* newChild) { children_[index] = newChild; }
	inline void SetBodyList(Body* newList) { bodyList_ = newList; }
	inline void SetBodyListEnd(Body* newListEnd) { bodyListEnd_ = newListEnd; }

	static int maxListSize;
	static int totalCollisions;

private:


	// Sphere to sphere collision test
	bool SphereToSphereCollision(Body* b1, Body* b2);

	int CollisionCreateTasks(Body* ancestorList[], ThreadFarm* farm, int bodyNumPerTask, Channel<CollisionEvent*>* collisionEventChannel);

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

	// list of bodies at this node for collision
	Body* bodyList_;
	Body* bodyListEnd_;

	// isExternal is true if this is this partition only contains 1 body
	bool isExternal_;

	// number of bodies in branch of tree
	int numBodies_;

	// depth of node
	int depth_;


	Channel<CollisionEvent*> collisionEventsChannel_;

};

