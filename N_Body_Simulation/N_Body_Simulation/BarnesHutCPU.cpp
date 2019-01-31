#include "BarnesHutCPU.h"

#include "Body.h"


BarnesHutCPU::BarnesHutCPU()
{
}


BarnesHutCPU::~BarnesHutCPU()
{
}


void BarnesHutCPU::Init() {


	origin_ = Partition(sf::Vector3f(0.0f, 0.0f, 0.0f), 1.0E12f);


}

void BarnesHutCPU::CleanUp() {


}


void BarnesHutCPU::TimeStep(float dt) {


	// Partition physics space
	PartitionTree tree(origin_);

	for (auto body : bodies_) {

		tree.Insert(body);
	}

	// loop for each body and traverse partition tree

	for (auto body : bodies_) {

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
