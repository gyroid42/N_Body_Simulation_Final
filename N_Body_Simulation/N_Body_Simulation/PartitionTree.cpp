#include "PartitionTree.h"
#include "Body.h"
#include "PhysicsUtil.h"


PartitionTree::PartitionTree(Partition newPartition)
{
	for (int i = 0; i < 8; i++) {

		children_[i] = nullptr;
	}

	partition_ = newPartition;
	isExternal_ = true;
}


PartitionTree::~PartitionTree()
{

	// loop for each child node and delete it
	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			delete children_[i];
			children_[i] = nullptr;
		}
	}
}



void PartitionTree::Insert(Body* body) {

	// if no body here add new body
	if (totalMass_ == 0.0f) {

		totalMass_ = body->Mass();
		centerOfMass_ = body->Position();
	}

	// if body is present and this isn't an external node
	// add the new body to the total mass and calculate new center of mass
	// then determine which octant the new body belongs in and add the body to it
	else if (!isExternal_) {

		// calculate center of mass and new total mass
		centerOfMass_ = (totalMass_ * centerOfMass_ + body->Mass() * body->Position()) / (totalMass_ + body->Mass());
		totalMass_ += body->Mass();

		// loop for each octant in partition
		for (int i = 0; i < 8; i++) {

			// get current partition
			Partition newPartition = partition_.GetSubDivision(i);

			// Check if body is in current partition
			if (newPartition.Contains(body->Position())) {

				// If current octant not being used add a new node
				if (children_[i] == nullptr) {

					children_[i] = new PartitionTree(newPartition);
				}

				// Insert body into octant found
				children_[i]->Insert(body);

				// exit the loop since we've found the octant the body belongs in
				break;
			}
		}
	}

	// If node is external then put the body in this current node into a child node
	else if (isExternal_) {

		// create a body from current nodes' total mass and center of mass
		Body* oldBody = new Body();
		oldBody->Init(centerOfMass_, sf::Vector3f(0.0f, 0.0f, 0.0f), totalMass_);

		// find which octant the body belongs in
		for (int i = 0; i < 8; i++) {

			Partition newPartition = partition_.GetSubDivision(i);

			if (newPartition.Contains(oldBody->Position())) {


				// if node doesn't exist yet create one
				if (children_[i] == nullptr) {

					children_[i] = new PartitionTree(newPartition);
				}

				// add body to the new node
				children_[i]->Insert(oldBody);

				// this node is no longer external
				isExternal_ = false;

				// exit the loop since we've found the octant the body belongs in
				break;
			}
		}

		// re-insert the body into this node
		Insert(body);
	}
}



void PartitionTree::UpdateForceOnBody(Body* body) {

	// If this node is external then add force due to this node since no more child nodes to check
	if (isExternal_) {

		body->AddForce(centerOfMass_, totalMass_);
	}

	// check if distance and length is appropriate to use this node
	else if (partition_.Length() / PhysicsUtil::DistanceTo(centerOfMass_, body->Position()) < 2.0f) {

		body->AddForce(centerOfMass_, totalMass_);
	}

	// node wasn't external or appropriate to use so check the children of this node
	else {

		for (int i = 0; i < 8; i++) {

			if (children_[i]) {

				children_[i]->UpdateForceOnBody(body);
			}
		}
	}
}


