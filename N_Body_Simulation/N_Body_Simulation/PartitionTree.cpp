
// class header include
#include "PartitionTree.h"

// standard library include
#include <iostream>

// my class includes
#include "Body.h"
#include "PhysicsUtil.h"


PartitionTree::PartitionTree(Partition newPartition) :
	body_(nullptr)
{

	// set all children to null
	for (int i = 0; i < 8; i++) {

		children_[i] = nullptr;
	}

	// set partition
	partition_ = newPartition;

	// tree just created therefore it is external
	isExternal_ = true;

	// set default total mass and center of mass
	totalMass_ = 0.0f;
	centerOfMass_ = sf::Vector3f(0.0f, 0.0f, 0.0f);


	numBodies_ = 0;
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



void PartitionTree::AddBody(Body* body) {


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


void PartitionTree::Insert(Body* body) {

	////////////// TO DO //////////////
	//								 //
	//	MAKE THIS METHOD THREAD SAFE //
	//								 //
	//	isExternal not safe			 //
	//	total mass and CoM not safe	 //
	//	children_[i] not safe		 //
	//								 //
	///////////////////////////////////

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

		//totalMass_ += body->Mass();

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
		//Body* oldBody = new Body();
		//oldBody->Init(centerOfMass_, sf::Vector3f(0.0f, 0.0f, 0.0f), totalMass_);

		// find which octant the body belongs in
		for (int i = 0; i < 8; i++) {

			Partition newPartition = partition_.GetSubDivision(i);

			if (newPartition.Contains(body_->Position())) {


				// if node doesn't exist yet create one
				if (children_[i] == nullptr) {

					children_[i] = new PartitionTree(newPartition);
				}

				// add body to the new node
				children_[i]->Insert(body_);

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



void PartitionTree::InsertMulti(Body* body) {

	// if no body here add new body
	totalMassMutex_.lock();
	if (totalMass_ == 0.0f) {

		totalMass_ = body->Mass();
		centerOfMass_ = body->Position();
		body_ = body;
		numBodies_++;

		totalMassMutex_.unlock();
	}
	else {


		totalMassMutex_.unlock();

		isExternalMutex_.lock();
		// If node is external then put the body in this current node into a child node
		if (isExternal_) {

			// this node is no longer external
			isExternal_ = false;

			isExternalMutex_.unlock();

			// find which octant the body belongs in
			for (int i = 0; i < 8; i++) {

				Partition newPartition = partition_.GetSubDivision(i);

				if (newPartition.Contains(body_->Position())) {

					{
						std::unique_lock<std::mutex> lck(childrenMutex_[i]);
						// if node doesn't exist yet create one
						if (children_[i] == nullptr) {

							children_[i] = new PartitionTree(newPartition);
						}
					}

					// add body to the new node
					children_[i]->InsertMulti(body_);

					// exit the loop since we've found the octant the body belongs in
					break;
				}
			}

			// re-insert the body into this node
			InsertMulti(body);
		}
		// if body is present and this isn't an external node
		// add the new body to the total mass and calculate new center of mass
		// then determine which octant the new body belongs in and add the body to it
		else {


			isExternalMutex_.unlock();

			totalMassMutex_.lock();

			// calculate center of mass and new total mass
			//centerOfMass_ = (totalMass_ * centerOfMass_ + body->Mass() * body->Position()) / (totalMass_ + body->Mass());

			sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
			sf::Vector3f bodyMassTimesPosition = body->Mass() * body->Position();
			totalMass_ += body->Mass();
			centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
			centerOfMass_ /= totalMass_;
			numBodies_++;


			totalMassMutex_.unlock();
			//totalMass_ += body->Mass();

			// loop for each octant in partition
			for (int i = 0; i < 8; i++) {

				// get current partition
				Partition newPartition = partition_.GetSubDivision(i);

				// Check if body is in current partition
				if (newPartition.Contains(body->Position())) {

					{
						std::unique_lock<std::mutex> lck(childrenMutex_[i]);
						// If current octant not being used add a new node
						if (children_[i] == nullptr) {

							children_[i] = new PartitionTree(newPartition);
						}

					}
					// Insert body into octant found
					children_[i]->InsertMulti(body);

					// exit the loop since we've found the octant the body belongs in
					break;
				}
			}
		}

		
	}

	
}



void PartitionTree::UpdateForceOnBody(Body* body) {

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

					children_[i]->UpdateForceOnBody(body);
				}
			}
		}
	}
}



void PartitionTree::Merge(PartitionTree* mergeTree) {


	if (totalMass_ == 0.0f) {

		totalMass_ = mergeTree->Mass();
		centerOfMass_ = mergeTree->CenterOfMass();
		body_ = mergeTree->GetBody();
	}
	else if (!isExternal_ && !mergeTree->IsExternal()) {

		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = mergeTree->Mass() * mergeTree->CenterOfMass();
		totalMass_ += mergeTree->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;

		for (int i = 0; i < 8; i++) {

			PartitionTree* mergeChild = mergeTree->GetChild(i);

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
	else if (isExternal_) {

		// sort out the external poop
		// merge children
		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = mergeTree->Mass() * mergeTree->CenterOfMass();
		totalMass_ += mergeTree->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;

		for (int i = 0; i < 8; i++) {

			PartitionTree* mergeChild = mergeTree->GetChild(i);

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


		for (int i = 0; i < 8; i++) {

			Partition newPartition = partition_.GetSubDivision(i);

			if (newPartition.Contains(body_->Position())) {

				if (!children_[i]) {

					children_[i] = new PartitionTree(newPartition);
				}

				children_[i]->Insert(body_);

				isExternal_ = false;

				//std::cout << i << std::endl;

				break;
			}
		}


	}
	else {


		sf::Vector3f totalTimesCenter = totalMass_ * centerOfMass_;
		sf::Vector3f bodyMassTimesPosition = mergeTree->Mass() * mergeTree->CenterOfMass();
		totalMass_ += mergeTree->Mass();
		centerOfMass_ = totalTimesCenter + bodyMassTimesPosition;
		centerOfMass_ /= totalMass_;

		for (int i = 0; i < 8; i++) {

			Partition newPartition = partition_.GetSubDivision(i);

			if (newPartition.Contains(body_->Position())) {

				if (!children_[i]) {

					children_[i] = new PartitionTree(newPartition);
				}

				children_[i]->Insert(mergeTree->GetBody());

				break;
			}
		}
		 
	}

	


}


void PartitionTree::CreateChildren() {


	for (int i = 0; i < 8; i++) {

		if (!children_[i]) {

			children_[i] = new PartitionTree(partition_.GetSubDivision(i));
		}
	}

	isExternal_ = false;
}


PartitionTree* PartitionTree::GetChild(int index) {

	return children_[index];
}




