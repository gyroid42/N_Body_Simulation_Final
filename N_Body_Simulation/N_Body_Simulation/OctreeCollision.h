#pragma once

// SFML Includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <vector>

// my class includes
#include "Partition.h"

// forward declarations
class Body;

class OctreeCollision
{

	struct Node {
		Partition partition;
		Node* children[8];
		Body* bodyList;
	};
public:
	OctreeCollision();
	~OctreeCollision();



};

