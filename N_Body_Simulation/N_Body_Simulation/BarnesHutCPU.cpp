
// include class header
#include "BarnesHutCPU.h"

// standard library includes
#include <iostream>

// my class includes
#include "Body.h"
#include "PartitionTree.h"


BarnesHutCPU::BarnesHutCPU()
{
}


BarnesHutCPU::~BarnesHutCPU()
{
}


void BarnesHutCPU::Init() {

	BarnesHut::Init();

	// Create the partition tree root
	root_ = Partition(sf::Vector3f(0.0f, 0.0f, 0.0f), 10000.0f);

	
}

void BarnesHutCPU::CleanUp() {

	BarnesHut::CleanUp();

}


void BarnesHutCPU::TimeStep(float dt) {


	// Partition physics space

	// create intial tree using partition root
	PartitionTree tree(root_);

	// insert each body into the partition tree
	for (auto body : bodies_) {

		tree.Insert(body);
	}


	// Calculate for acting on each body

	// loop for each body and traverse partition tree
	for (auto body : bodies_) {

		// reset body force before applying forces
		body->ResetForce();

		tree.UpdateForceOnBody(body);
	}

	// loop for each body and integrate

	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}
}


void BarnesHutCPU::PartitionSpace() {


}


void BarnesHutCPU::CalculateForceOnBody(Body* body) {


}
