#pragma once

// SFML Includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <vector>

// my class includes
#include "Partition.h"

// forward declarations
class Body;

struct CollisionNode {
	Partition partition;
	CollisionNode* children[8];
	Body* bodyList;

	CollisionNode() {

		for (int i = 0; i < 8; i++) {

			children[i] = nullptr;
		}

		bodyList = nullptr;
	}

	~CollisionNode() {

		for (int i = 0; i < 8; i++) {

			if (children[i]) {

				delete children[i];
				children[i] = nullptr;
			}
		}
	}
};

class OctreeCollision
{

public:
	OctreeCollision();
	~OctreeCollision();


	CollisionNode* Build(Partition partition, int stopDepth);

	void Insert(CollisionNode* node, Body* body);

	void TestAllCollisions(CollisionNode* node);
	void TestCollision(Body* b1, Body* b2);

	
private:

	bool SphereToSphere(Body* b1, Body* b2);
};

