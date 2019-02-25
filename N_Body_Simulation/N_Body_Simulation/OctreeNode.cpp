
// class header include
#include "OctreeNode.h"

// standard library include
#include <iostream>

// my class includes
#include "Body.h"
#include "PhysicsUtil.h"

OctreeNode::OctreeNode(Partition newPartition) :
	body_(nullptr),
	treeRoot_(nullptr)
{

	// set all children to null
	for (int i = 0; i < 8; i++) {

		children_[i] = nullptr;
	}

	// set partition
	partition_ = newPartition;

	// tree just created therefore it is external
	isExternal_ = true;

	isLocalised_ = true;

	// set default total mass and center of mass
	totalMass_ = 0.0f;
	centerOfMass_ = sf::Vector3f(0.0f, 0.0f, 0.0f);


	numBodies_ = 0;

	treeRoot_ = this;
}

OctreeNode::OctreeNode(Partition newPartition, OctreeNode* newRoot) :
	body_(nullptr),
	treeRoot_(newRoot)
{
	// set all children to null
	for (int i = 0; i < 8; i++) {

		children_[i] = nullptr;
	}

	// set partition
	partition_ = newPartition;

	// tree just created therefore it is external
	isExternal_ = true;

	isLocalised_ = true;

	// set default total mass and center of mass
	totalMass_ = 0.0f;
	centerOfMass_ = sf::Vector3f(0.0f, 0.0f, 0.0f);


	numBodies_ = 0;
}


OctreeNode::~OctreeNode()
{

	// loop for each child node and delete it
	if (isLocalised_) {

		for (int i = 0; i < 8; i++) {

			if (children_[i]) {

				delete children_[i];
				children_[i] = nullptr;
			}
		}
	}
}



void OctreeNode::AddBody(Body* body) {


	if (totalMass_ == 0.0f) {

		totalMass_ = body->Mass();
		centerOfMass_ = body->Position();
		body_ = body;
	}
	else {

		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = body->Mass() * body->Position();
		totalMass_ += body->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;
	}

	numBodies_++;
}


void OctreeNode::Insert(Body* body, int& counter) {


	counter++;

	if (counter > 500) {

		int poop = 0;
	}

	// if no body here add new body
	if (totalMass_ == 0.0f) {

		totalMass_ = body->Mass();
		centerOfMass_ = body->Position();
		body_ = body;
		numBodies_++;
	}

	// if body is present and this isn't an external node
	// add the new body to the total mass and calculate new center of mass
	// then determine which octant the new body belongs in and add the body to it
	else if (!isExternal_) {

		// calculate center of mass and new total mass
		//centerOfMass_ = (totalMass_ * centerOfMass_ + body->Mass() * body->Position()) / (totalMass_ + body->Mass());

		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = body->Mass() * body->Position();
		totalMass_ += body->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;
		numBodies_++;


		// loop for each octant in partition
		for (int i = 0; i < 8; i++) {

			// get current partition
			Partition newPartition = partition_.GetSubDivision(i);

			// Check if body is in current partition
			if (newPartition.Contains(body->Position())) {

				// If current octant not being used add a new node
				if (children_[i] == nullptr) {

					children_[i] = new OctreeNode(newPartition, treeRoot_);
				}

				// Insert body into octant found
				children_[i]->Insert(body, counter);

				// exit the loop since we've found the octant the body belongs in
				break;
			}
		}
	}

	// If node is external then put the body in this current node into a child node
	else if (isExternal_) {

		// find which octant the body belongs in
		for (int i = 0; i < 8; i++) {

			Partition newPartition = partition_.GetSubDivision(i);

			if (newPartition.Contains(body_->Position())) {


				// if node doesn't exist yet create one
				if (children_[i] == nullptr) {

					children_[i] = new OctreeNode(newPartition, treeRoot_);
				}

				// add body to the new node
				children_[i]->Insert(body_, counter);

				// this node is no longer external
				isExternal_ = false;

				// exit the loop since we've found the octant the body belongs in
				break;
			}
		}

		// re-insert the body into this node
		Insert(body, counter);
	}

	counter++;
}



void OctreeNode::UpdateForceOnBody(Body* body) {

	// If this node is external then add force due to this node since no more child nodes to check
	if (totalMass_ != 0.0f) {

		if (isExternal_) {

			if (body != body_) {

				body->AddForce(centerOfMass_, totalMass_);
			}
		}

		// check if distance and length is appropriate to use this node
		else if (partition_.Length() / PhysicsUtil::DistanceTo(centerOfMass_, body->Position()) < 1.0f) {

			body->AddForce(centerOfMass_, totalMass_);
		}

		// node wasn't external or appropriate to use so check the children of this node
		else {

			for (int i = 0; i < 8; i++) {

				if (children_[i]) {

					if (!children_[i]->IsLocalised()) {

						children_[i]->Localise();
					}
					children_[i]->UpdateForceOnBody(body);
				}
			}
		}
	}
}



void OctreeNode::Merge(OctreeNode* mergeTree) {


	// If this node is empty
	if (totalMass_ == 0.0f) {

		// Set this node to be the same as mergeTree
		totalMass_ = mergeTree->Mass();
		centerOfMass_ = mergeTree->CenterOfMass();
		body_ = mergeTree->GetBody();
		numBodies_ = mergeTree->NumBodies();

		isExternal_ = mergeTree->IsExternal();

		// Set all children to mergeTree children
		for (int i = 0; i < 8; i++) {

			children_[i] = mergeTree->GetChild(i);
			mergeTree->SetChild(i, nullptr);
		}
	}

	// Else if both this node and mergeTree node are not external
	else if (!isExternal_ && !mergeTree->IsExternal()) {

		// combine nodes together
		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = mergeTree->Mass() * mergeTree->CenterOfMass();
		totalMass_ += mergeTree->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;
		numBodies_ += mergeTree->NumBodies();

		// Loop for each child and merge with the mergeTrees children
		for (int i = 0; i < 8; i++) {

			OctreeNode* mergeChild = mergeTree->GetChild(i);

			if (mergeChild) {
				if (!children_[i]) {

					children_[i] = mergeTree->GetChild(i);
					mergeTree->SetChild(i, nullptr);
				}
				else {

					children_[i]->Merge(mergeTree->GetChild(i));
				}
			}
		}

	}
	else {


		// If either node is external

		// combine both nodes together
		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = mergeTree->Mass() * mergeTree->CenterOfMass();
		totalMass_ += mergeTree->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;
		numBodies_ += mergeTree->NumBodies();

		// If this node is external
		if (isExternal_) {

			// loop for each of the merge trees child and merge it with this nodes children
			for (int i = 0; i < 8; i++) {

				OctreeNode* mergeChild = mergeTree->GetChild(i);

				if (mergeChild) {
					if (!children_[i]) {

						children_[i] = mergeTree->GetChild(i);
						mergeTree->SetChild(i, nullptr);
					}
					else {

						children_[i]->Merge(mergeTree->GetChild(i));
					}
				}
			}


			// loop for each child and insert this nodes body into the correct one
			for (int i = 0; i < 8; i++) {

				// check if body is in current subdivision
				Partition newPartition = partition_.GetSubDivision(i);

				if (newPartition.Contains(body_->Position())) {

					// if it is then insert it into that child
					if (!children_[i]) {

						children_[i] = new OctreeNode(newPartition, treeRoot_);
					}

					int counter = 0;

					children_[i]->Insert(body_, counter);

					// this node is no longer external
					isExternal_ = false;

					break;
				}
			}


		}

		// If the merge tree node is external
		if (mergeTree->IsExternal()) {


			// loop for each child and insert the merge tree node's body into the correct one
			for (int i = 0; i < 8; i++) {

				// check if body is in current subdivision
				Partition newPartition = partition_.GetSubDivision(i);

				if (newPartition.Contains(mergeTree->GetBody()->Position())) {

					// if it is then insert it into that child
					if (!children_[i]) {

						children_[i] = new OctreeNode(newPartition, treeRoot_);
					}

					int counter = 0;
					children_[i]->Insert(mergeTree->GetBody(), counter);

					break;
				}
			}

		}
	}

}


void OctreeNode::GetOrderedElementsList(std::vector<Body*>& newList) {

	// Will traverse tree until it gets to an external node and add that body to the list
	// It'll then goto the next node etc.. until the whole tree has been traversed

	//If this node is external and contains a body
	if (isExternal_ && body_) {

		// only add body if it's within the physics space
		if (treeRoot_->GetPartition().Contains(body_->Position())) {

			// add it to the new body list
			newList.push_back(body_);
		}
		else {

			int pee = 0;
		}
	}
	else {

		// else loop for each child and recursively call this method
		for (int i = 0; i < 8; i++) {

			if (children_[i]) {

				children_[i]->GetOrderedElementsList(newList);
			}
		}
	}
}


void OctreeNode::CreateChildren() {

	// loop for each child and create a node there
	for (int i = 0; i < 8; i++) {

		if (!children_[i]) {

			children_[i] = new OctreeNode(partition_.GetSubDivision(i), treeRoot_);
		}
	}

	isExternal_ = false;
}


OctreeNode* OctreeNode::GetChild(int index) {

	return children_[index];
}

OctreeNode* OctreeNode::GetCopy() {

	OctreeNode* localisedCopy = new OctreeNode(partition_);

	localisedCopy->SetLocalised(false);
	localisedCopy->SetBody(body_);
	localisedCopy->SetCoM(centerOfMass_);
	localisedCopy->SetExternal(isExternal_);
	localisedCopy->SetMass(totalMass_);

	for (int i = 0; i < 8; i++) {

		localisedCopy->SetChild(i, children_[i]);
	}

	return localisedCopy;
}

void OctreeNode::Localise() {

	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			children_[i] = children_[i]->GetCopy();
		}
	}

	isLocalised_ = true;
}


