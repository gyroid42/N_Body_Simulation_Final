#include "OctreeCollision.h"

#include "Body.h"
#include "PhysicsUtil.h"

#include <iostream>

OctreeCollision::OctreeCollision()
{
}


OctreeCollision::~OctreeCollision()
{
}


CollisionNode* OctreeCollision::Build(Partition partition, int stopDepth) {

	
	if (stopDepth < 0) {

		return nullptr;
	}
	
	CollisionNode* node = new CollisionNode();
	node->partition = partition;
	
	for (int i = 0; i < 8; i++) {
		
		node->children[i] = Build(partition.GetSubDivision(i), stopDepth - 1);
	}

	return node;
}

void OctreeCollision::Insert(CollisionNode* node, Body* body) {

	int index = 0, straddle = 0;
	// Compute the octant number [0..7] the object sphere center is in
	// If straddling any of the dividing x, y, or z planes, exit directly

	float delta[3];
	delta[0] = body->Position().x - node->partition.Position().x;
	delta[1] = body->Position().y - node->partition.Position().y;
	delta[2] = body->Position().z - node->partition.Position().z;

	for (int i = 0; i < 3; i++) {

		if (abs(delta[i]) <= body->ModelRadius()) {

			straddle = 1;

			break;
		}
		if (delta[i] > 0.0f) {
			index |= (1 << i);
		}
	}
	
	if (!straddle && node->children[index]) {
		// Fully contained in existing child node; insert in that subtree
		Insert(node->children[index], body);
	}
	else {
		// straddling, or no child node to descend into, so
		// link object into vector at this node
		body->SetNextBody(node->bodyList);
		node->bodyList = body;
	}


}



void OctreeCollision::TestAllCollisions(CollisionNode* node) {

	// Keep track of all ancestor object lists in a stack
	const int MAX_DEPTH = 40;
	static CollisionNode* ancestorStack[MAX_DEPTH];
	static int depth = 0; // Depth ==0 is invariant over calls

	// Check collision between all objects on this level and all 
	// ancestor objects. The current level is included as it's own
	// ancestor so all necessary pairwise tests are done
	ancestorStack[depth++] = node;
	for (int n = 0; n < depth; n++) {

		Body *b0, *b1;
		for (b0 = ancestorStack[n]->bodyList; b0; b0 = b0->NextBody()) {

			for (b1 = node->bodyList; b1; b1 = b1->NextBody()) {
				// avoid testing both bl0 -> bl1 and bl1 -> bl1
				if (b0 == b1) {
					break;
				}
				// Now perform the collision test between bl0 and bl1 in some manner
				TestCollision(b0, b1);
			}
		}
		//std::cout << "poop" << std::endl;
	}

	// recursively visit all existing children
	for (int i = 0; i < 8; i++) {
		if (node->children[i]) {
			TestAllCollisions(node->children[i]);
		}
	}

	// Remove the current node from the ancestor stack before returning
	depth--;
}



void OctreeCollision::TestCollision(Body* b1, Body* b2) {

	if (SphereToSphere(b1, b2)) {

		int gay = 0;
		//b1->OnCollision(b2);
		//b2->OnCollision(b1);
	}
}


bool OctreeCollision::SphereToSphere(Body* b1, Body* b2) {

	float distance = PhysicsUtil::DistanceToSqr(b1->Position(), b2->Position());
	float radius = b1->ModelRadius() + b2->ModelRadius();

	return distance <= radius * radius;
}

