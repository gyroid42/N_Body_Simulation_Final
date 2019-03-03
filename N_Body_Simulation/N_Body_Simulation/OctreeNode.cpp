
// class header include
#include "OctreeNode.h"

// standard library include
#include <iostream>

// my class includes
#include "Body.h"
#include "PhysicsUtil.h"

int OctreeNode::maxListSize = 0;

OctreeNode::OctreeNode(Partition newPartition) :
	body_(nullptr),
	bodyList_(nullptr),
	bodyListEnd_(nullptr),
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

	// set default total mass and center of mass
	totalMass_ = 0.0f;
	centerOfMass_ = sf::Vector3f(0.0f, 0.0f, 0.0f);

	// node starts with 0 bodies
	numBodies_ = 0;

	// no root given therefore this node is the root
	treeRoot_ = this;

	// this is root node therefore depth is 0
	depth_ = 0;
}

OctreeNode::OctreeNode(Partition newPartition, OctreeNode* newRoot, int newDepth) :
	body_(nullptr),
	bodyList_(nullptr),
	bodyListEnd_(nullptr),
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

	// set default total mass and center of mass
	totalMass_ = 0.0f;
	centerOfMass_ = sf::Vector3f(0.0f, 0.0f, 0.0f);

	// nodes starts with 0 bodies
	numBodies_ = 0;

	// set depth
	depth_ = newDepth;
}


OctreeNode::~OctreeNode()
{

	// loop for each child node and delete it
	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			delete children_[i];
			children_[i] = nullptr;
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


void OctreeNode::Insert(Body* body, int& depthCounter) {

	depthCounter++;

	if (depthCounter > 500) {

		int poop = 0;
	}

	// if no body here add new body
	if (totalMass_ == 0.0f) {

		totalMass_ = body->Mass();
		centerOfMass_ = body->Position();
		body_ = body;
		numBodies_++;

		// If body hasn't been inserted to lowest collision depth yet
		if (!body->InsertedCollision()) {

			// set body list to new body
			bodyList_ = body;
			bodyListEnd_ = body;
		}
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

					children_[i] = new OctreeNode(newPartition, treeRoot_, depth_ + 1);
				}

				// If body hasn't been inserted to lowest collision depth yet
				if (!body->InsertedCollision()) {

					// Check if body is straddling edge of node
					if (partition_.StraddleCheck(body->Position(), body->ModelRadius())) {

						// Body is straddling therefore this is the lowest depth for body
						// Add body to bodyList
						if (bodyList_) {
							body->SetNextBody(bodyList_);
						}
						else {
							bodyListEnd_ = body;
						}
						bodyList_ = body;

						body->SetInsertedCollision(true);
					}
				}
				

				// Insert body into octant found
				children_[i]->Insert(body, depthCounter);

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

					children_[i] = new OctreeNode(newPartition, treeRoot_, depth_ + 1);
				}
				
				// If there is a body in current bodyList
				if (bodyList_) {

					// there is a body so check if it is at it's lowest depth
					if (partition_.StraddleCheck(bodyList_->Position(), bodyList_->ModelRadius())) {

						// It is straddling so flag it to no longer be checked in deeper nodes
						bodyList_->SetInsertedCollision(true);
					}
					else {

						// remove body from body list as it's being inserted into a deeper bodyList
						bodyList_ = nullptr;
						bodyListEnd_ = nullptr;
					}
				}
				
				// add body to the new node
				children_[i]->Insert(body_, depthCounter);

				// this node is no longer external
				isExternal_ = false;

				// exit the loop since we've found the octant the body belongs in
				break;
			}
		}

		// re-insert the body into this node
		Insert(body, depthCounter);
	}

	depthCounter--;
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
		else if ((partition_.HalfLength() * 2.0f) / PhysicsUtil::DistanceTo(centerOfMass_, body->Position()) < 1.0f) {

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



void OctreeNode::Merge(OctreeNode* mergeTree) {


	// If this node is empty
	if (totalMass_ == 0.0f) {

		// Set this node to be the same as mergeTree
		totalMass_ = mergeTree->Mass();
		centerOfMass_ = mergeTree->CenterOfMass();
		body_ = mergeTree->GetBody();
		numBodies_ = mergeTree->NumBodies();

		bodyList_ = mergeTree->GetBodyList();
		bodyListEnd_ = mergeTree->GetBodyListEnd();

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

		// If there's a bodyList
		if (mergeTree->GetBodyList()) {
			if (bodyList_) {

				// Add the mergeTree bodylist onto the end of merged body list
				bodyListEnd_->SetNextBody(mergeTree->GetBodyList());
				bodyListEnd_ = mergeTree->GetBodyListEnd();
			}
			else {

				// Set body list to mergeTree's body list
				bodyList_ = mergeTree->GetBodyList();
				bodyListEnd_ = mergeTree->GetBodyListEnd();
			}
		}
		//body_->SetNextBody(mergeTree->GetBodyList());


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

						children_[i] = new OctreeNode(newPartition, treeRoot_, depth_ + 1);
					}

					// If body list contains a body
					if (bodyList_) {

						// check if it's straddling
						if (partition_.StraddleCheck(bodyList_->Position(), bodyList_->ModelRadius())) {

							// It is so flag the body not be added into deeper body lists
							bodyList_->SetInsertedCollision(true);
						}
						else {

							// body can go deeper so remove from body list
							bodyList_ = nullptr;
							bodyListEnd_ = nullptr;
						}
					}

					int counter = 0;

					children_[i]->Insert(body_, counter);

					// this node is no longer external
					isExternal_ = false;

					break;
				}
			}


		}

		////////////////////////////////////////////////
		/*
		// handle merge tree
		Body* mergeBodyList = mergeTree->GetBodyList();

		// if there is a body list in the merge tree
		if (mergeBodyList) {

			// check if body in list can be inserted
			if (!mergeBodyList->InsertedCollision()) {

				// do a straddle check on merge body
				if (partition_.StraddleCheck(mergeBodyList->Position(), mergeBodyList->ModelRadius())) {

					// merge body is straddling so add to body list
					

					mergeBodyList->SetNextBody(bodyList_);
					bodyList_ = mergeBodyList;

					mergeBodyList->SetInsertedCollision(true);
				}
				else {

					// body can go deeper, so remove from merge body list
					mergeBodyList = nullptr;
				}
			}
		}
		*/
		//////////////////////////////////////////////////////////

		// If the merge tree node is external
		if (mergeTree->IsExternal()) {

			Body* body = mergeTree->GetBody();

			// loop for each child and insert the merge tree node's body into the correct one
			for (int i = 0; i < 8; i++) {

				// check if body is in current subdivision
				Partition newPartition = partition_.GetSubDivision(i);

				if (newPartition.Contains(body->Position())) {

					// if it is then insert it into that child
					if (!children_[i]) {

						children_[i] = new OctreeNode(newPartition, treeRoot_, depth_ + 1);
					}

					Body* mergeBodyList = mergeTree->GetBodyList();

					if (mergeBodyList) {

						if (!mergeBodyList->InsertedCollision()) {

							// do a straddle check on merge body
							if (partition_.StraddleCheck(mergeBodyList->Position(), mergeBodyList->ModelRadius())) {

								// merge body is straddling so add to body list

								if (bodyList_) {

									mergeBodyList->SetNextBody(bodyList_);
								}
								else {

									bodyListEnd_ = mergeTree->GetBodyListEnd();
								}
								bodyList_ = mergeBodyList;

								mergeBodyList->SetInsertedCollision(true);
							}

							// Body is has either been already merged into bodylist or is being placed deeper
							// so set mergeTree's body list to nullptr
							mergeTree->SetBodyList(nullptr);
							mergeTree->SetBodyListEnd(nullptr);
						}
					}

					int counter = 0;
					children_[i]->Insert(body, counter);

					break;
				}
			}

		}

		// combine the two merge lists
		if (mergeTree->GetBodyList()) {
			if (bodyList_) {

				// Add the mergeTree bodylist onto the end of merged body list
				bodyListEnd_->SetNextBody(mergeTree->GetBodyList());
				bodyListEnd_ = mergeTree->GetBodyListEnd();
			}
			else {

				// Set body list to mergeTree's body list
				bodyList_ = mergeTree->GetBodyList();
				bodyListEnd_ = mergeTree->GetBodyListEnd();
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
		if (!body_->DestroyFlag() && treeRoot_->GetPartition().Contains(body_->Position())) {

			body_->SetInsertedCollision(false);
			body_->SetNextBody(nullptr);
			// add it to the new body list
			newList.push_back(body_);
		}
		else {

			delete body_;
			body_ = nullptr;
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

			children_[i] = new OctreeNode(partition_.GetSubDivision(i), treeRoot_, depth_ + 1);
		}
	}

	isExternal_ = false;
}


OctreeNode* OctreeNode::GetChild(int index) {

	return children_[index];
}




void OctreeNode::CheckAllCollision(Body* ancestorList[], unsigned short int depth) {
	
	ancestorList[depth] = bodyList_;

	depth++;

	int poop = 0;

	for (int i = 0; i < depth; i++) {

		Body* b1;
		Body* b2;


		for (b1 = ancestorList[i]; b1; b1 = b1->NextBody()) {

			poop++;

			for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

				if (b1 == b2) {

					break;
				}

				TestCollision(b1, b2);
			}
		}
	}

	if (poop > maxListSize) {

		maxListSize = poop;
	}


	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			children_[i]->CheckAllCollision(ancestorList, depth);
		}
	}

	depth--;
}


void OctreeNode::CheckCollisionSingleNode(Body* comparisonList[]) {

	Body* b1;
	Body* b2;

	for (b1 = comparisonList[i]; b1; b1 = b1->NextBody()) {

		for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

			if (b1 == b2) {

				break;
			}

			TestCollision(b1, b2);
		}
	}
}


int OctreeNode::totalCollisions = 0;

void OctreeNode::TestCollision(Body* b1, Body* b2) {

	if (SphereToSphereCollision(b1, b2)) {

		int poop = 0;
		totalCollisions++;
	}
}


bool OctreeNode::SphereToSphereCollision(Body* b1, Body* b2) {

	float distance = PhysicsUtil::DistanceToSqr(b1->Position(), b2->Position());
	float radius = b1->ModelRadius() + b2->ModelRadius();

	return distance <= radius * radius;
}


void OctreeNode::CollisionBegin() {

	totalCollisions = 0;


	Body* ancestorList[50];

	CheckAllCollision(ancestorList, 0);
}

