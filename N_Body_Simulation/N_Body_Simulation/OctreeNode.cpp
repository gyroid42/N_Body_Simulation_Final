
// class header include
#include "OctreeNode.h"

// standard library include
#include <iostream>

// my class includes
#include "Body.h"
#include "PhysicsUtil.h"
#include "TaskCollisionCheckNode.h"
#include "TaskCollisionCheckTree.h"
#include "ThreadFarm.h"
#include "CollisionEvent.h"

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


void OctreeNode::UpdateForceOnBody(Body* body, float& theta, int& totalForceCalcs) {

	// If this node is external then add force due to this node since no more child nodes to check
	if (totalMass_ != 0.0f) {

		if (isExternal_) {

			if (body != body_) {

				body->AddForce(centerOfMass_, totalMass_);
				totalForceCalcs++;
			}
		}

		// check if distance and length is appropriate to use this node
		else if ((partition_.HalfLength() * 2.0f) / PhysicsUtil::DistanceTo(centerOfMass_, body->Position()) < theta) {

			body->AddForce(centerOfMass_, totalMass_);
			totalForceCalcs++;
		}

		// node wasn't external or appropriate to use so check the children of this node
		else {

			for (int i = 0; i < 8; i++) {

				if (children_[i]) {

					children_[i]->UpdateForceOnBody(body, theta, totalForceCalcs);
				}
			}
		}
	}
}

void OctreeNode::UpdateForceOnBody(Body* body, float& theta) {

	// If this node is external then add force due to this node since no more child nodes to check
	if (totalMass_ != 0.0f) {

		if (isExternal_) {

			if (body != body_) {

				body->AddForce(centerOfMass_, totalMass_);
			}
		}

		// check if distance and length is appropriate to use this node
		else if ((partition_.HalfLength() * 2.0f) / PhysicsUtil::DistanceTo(centerOfMass_, body->Position()) < theta) {

			body->AddForce(centerOfMass_, totalMass_);
		}

		// node wasn't external or appropriate to use so check the children of this node
		else {

			for (int i = 0; i < 8; i++) {

				if (children_[i]) {

					children_[i]->UpdateForceOnBody(body, theta);
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




void OctreeNode::CheckAllCollision(Body* ancestorList[], CollisionEvent*& collisionEvents) {
	
	// add this nodes bodylist to the ancestor list
	ancestorList[depth_] = bodyList_;

	int ancestorListSize = 0;

	// only check for collisions if this node contains a body list
	if (bodyList_) {

		Body* b1;
		Body* b2;

		// for each ancestor list upto and including current depth
		for (int i = 0; i < depth_ + 1; i++) {

			// check each body in current ancestor list depth
			for (b1 = ancestorList[i]; b1; b1 = b1->NextBody()) {

				ancestorListSize++;

				// check each body in this node's depth
				for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

					// don't check collision if bodies are the same
					if (b1 == b2) {

						break;
					}

					// Check collision using Bounding Sphere
					if (TestCollision(b1, b2)) {

						// collision has happened so create a collision event
						// and add the collision event to list
						CollisionEvent* newCollision = new CollisionEvent(b1, b2);

						if (collisionEvents) {

							newCollision->next = collisionEvents;
						}

						collisionEvents = newCollision;
					}
				}
			}
		}
	}

	// if this list was larger than max, then new max found
	if (ancestorListSize > maxListSize) {

		maxListSize = ancestorListSize;
	}

	// check all the children of this node for collisions
	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			children_[i]->CheckAllCollision(ancestorList, collisionEvents);
		}
	}

}


void OctreeNode::CheckAllCollision(Body* ancestorList[], CollisionEvent*& collisionEvents, int& totalChecks) {

	// add this nodes bodylist to the ancestor list
	ancestorList[depth_] = bodyList_;

	int ancestorListSize = 0;

	// only check for collisions if this node contains a body list
	if (bodyList_) {

		Body* b1;
		Body* b2;

		// for each ancestor list upto and including current depth
		for (int i = 0; i < depth_ + 1; i++) {

			// check each body in current ancestor list depth
			for (b1 = ancestorList[i]; b1; b1 = b1->NextBody()) {

				ancestorListSize++;

				// check each body in this node's depth
				for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

					// don't check collision if bodies are the same
					if (b1 == b2) {

						break;
					}

					totalChecks++;

					// Check collision using Bounding Sphere
					if (TestCollision(b1, b2)) {

						// collision has happened so create a collision event
						// and add the collision event to list
						CollisionEvent* newCollision = new CollisionEvent(b1, b2);

						if (collisionEvents) {

							newCollision->next = collisionEvents;
						}

						collisionEvents = newCollision;
					}
				}
			}
		}
	}

	// if this list was larger than max, then new max found
	if (ancestorListSize > maxListSize) {

		maxListSize = ancestorListSize;
	}

	// check all the children of this node for collisions
	for (int i = 0; i < 8; i++) {

		if (children_[i]) {

			children_[i]->CheckAllCollision(ancestorList, collisionEvents, totalChecks);
		}
	}

}



// checks collision only for this node
void OctreeNode::CheckCollisionSingleNode(Body* ancestorList[], CollisionEvent*& collisionEvents) {

	Body* b1;
	Body* b2;

	// check ancestor list upto and including this nodes depth
	for (int i = 0; i < depth_ + 1; i++) {

		// check each body in current ancestor list depth
		for (b1 = ancestorList[i]; b1; b1 = b1->NextBody()) {

			// check each body in this node's depth
			for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

				// don't check collision if bodies are the same
				if (b1 == b2) {

					break;
				}

				// Check collision using Bounding Sphere
				if (TestCollision(b1, b2)) {

					// collision has happened so create a collision event
					// and add the collision event to list
					CollisionEvent* newCollision = new CollisionEvent(b1, b2);

					if (collisionEvents) {

						newCollision->next = collisionEvents;
					}

					collisionEvents = newCollision;
				}
			}
		}
	}
}


void OctreeNode::CheckCollisionSingleNode(Body* ancestorList[], CollisionEvent*& collisionEvents, int& totalChecks) {

	Body* b1;
	Body* b2;

	// check ancestor list upto and including this nodes depth
	for (int i = 0; i < depth_ + 1; i++) {

		// check each body in current ancestor list depth
		for (b1 = ancestorList[i]; b1; b1 = b1->NextBody()) {

			// check each body in this node's depth
			for (b2 = bodyList_; b2; b2 = b2->NextBody()) {

				// don't check collision if bodies are the same
				if (b1 == b2) {

					break;
				}

				totalChecks++;

				// Check collision using Bounding Sphere
				if (TestCollision(b1, b2)) {

					// collision has happened so create a collision event
					// and add the collision event to list
					CollisionEvent* newCollision = new CollisionEvent(b1, b2);

					if (collisionEvents) {

						newCollision->next = collisionEvents;
					}

					collisionEvents = newCollision;
				}
			}
		}
	}
}


int OctreeNode::totalCollisions = 0;

bool OctreeNode::TestCollision(Body* b1, Body* b2) {

	// check collision using sphere to sphere
	bool result = SphereToSphereCollision(b1, b2);

	if (result) {

		totalCollisions++;
	}

	return result;
}


bool OctreeNode::SphereToSphereCollision(Body* b1, Body* b2) {


	// x1*x2 + y1*y2 + z1*z2 <= (r1+r2)*(r1*r2)

	float distance = PhysicsUtil::DistanceToSqr(b1->Position(), b2->Position());
	float radius = b1->ModelRadius() + b2->ModelRadius();

	return distance <= radius * radius;
}



int OctreeNode::CollisionCreateTasks(Body* ancestorList[], ThreadFarm* farm, int bodyNumPerTask, Channel<CollisionEvent*>* collisionEventChannel, Channel<int>* checkCountChannel) {

	// int determines how many tasks have been created
	int tasksCreated = 1;

	// add this nodes bodylist to ancestor list
	ancestorList[depth_] = GetBodyList();

	// create a task to check this node
	TaskCollisionCheckNode* nodeCheckTask = new TaskCollisionCheckNode();

#if BENCHMARKING
	nodeCheckTask->Init(this, checkCountChannel, collisionEventChannel, ancestorList);
#else
	nodeCheckTask->Init(this, collisionEventChannel, ancestorList);
#endif

	farm->AddTask(nodeCheckTask);

	// for each child
	for (int i = 0; i < 8; i++) {

		// get child at i
		OctreeNode* child = GetChild(i);

		// if child exists
		if (child) {

			// only add a task if child contains <= (total Body Count) / (Number of Threads)
			if (child->NumBodies() <= bodyNumPerTask) {

				// create task to check entire tree below this child
				TaskCollisionCheckTree* treeCollisionTask = new TaskCollisionCheckTree();

#if BENCHMARKING
				treeCollisionTask->Init(child, checkCountChannel, collisionEventChannel, ancestorList);
#else
				treeCollisionTask->Init(child, collisionEventChannel, ancestorList);
#endif
				farm->AddTask(treeCollisionTask);

				// task has been created so add to counter
				tasksCreated++;
			}
			else {

				// too many bodies, create tasks in child
				tasksCreated += child->CollisionCreateTasks(ancestorList, farm, bodyNumPerTask, collisionEventChannel, checkCountChannel);
			}
		}
	}

	
	return tasksCreated;
}


void OctreeNode::CollisionCheckParallel(ThreadFarm* farm, int bodyNumPerTask) {


	// create ancestor list
	Body* ancestorList[MAX_COLLISION_DEPTH];


	// Create tasks for collision detection
	// Get number of tasks created
	int tasksCreated = CollisionCreateTasks(ancestorList, farm, bodyNumPerTask, &collisionEventsChannel_);

	

	CollisionEvent* collisionEvents = nullptr;

	// for number of tasks created
	for (int i = 0; i < tasksCreated; i++) {

		// get collision event from tasks
		CollisionEvent* newEvents = collisionEventsChannel_.read();

		// if there is a collisoin event
		if (newEvents) {

			// add to collision events list
			CollisionEvent* endList = collisionEvents;
			while (endList && endList->next) {

				endList = endList->next;
			}

			if (endList) {

				endList->next = newEvents;
			}
			else {

				collisionEvents = newEvents;
			}
		}
	}


	// all collision have been detected now and the events collected
	// safe to evaluate results of collisions

	// loop for every collision event that occured
	for (CollisionEvent* collision = collisionEvents; collision; collision = collision->next) {

		// handle collision here
#if COLLISION_REACTION == 0

		// Collision Reaction 0 is to merge the bodies in the collision

		if (collision->b1->Mass() > collision->b2->Mass()) {

			collision->b1->MergeBody(collision->b2);
			collision->b2->Destroy();
		}
		else {

			collision->b2->MergeBody(collision->b1);
			collision->b1->Destroy();
		}
#endif

	}

	// delete collision events
	if (collisionEvents) {

		delete collisionEvents;
		collisionEvents = nullptr;
	}
	
}


int OctreeNode::CollisionCheckParallel(ThreadFarm* farm, int bodyNumPerTask, Channel<int>* checkCountChannel) {


	// create ancestor list
	Body* ancestorList[MAX_COLLISION_DEPTH];


	// Create tasks for collision detection
	// Get number of tasks created
	int tasksCreated = CollisionCreateTasks(ancestorList, farm, bodyNumPerTask, &collisionEventsChannel_, checkCountChannel);



	CollisionEvent* collisionEvents = nullptr;

	// for number of tasks created
	for (int i = 0; i < tasksCreated; i++) {

		// get collision event from tasks
		CollisionEvent* newEvents = collisionEventsChannel_.read();

		// if there is a collisoin event
		if (newEvents) {

			// add to collision events list
			CollisionEvent* endList = collisionEvents;
			while (endList && endList->next) {

				endList = endList->next;
			}

			if (endList) {

				endList->next = newEvents;
			}
			else {

				collisionEvents = newEvents;
			}
		}
	}


	// all collision have been detected now and the events collected
	// safe to evaluate results of collisions

	// loop for every collision event that occured
	for (CollisionEvent* collision = collisionEvents; collision; collision = collision->next) {

		// handle collision here
#if COLLISION_REACTION == 0

		// Collision Reaction 0 is to merge the bodies in the collision

		if (collision->b1->Mass() > collision->b2->Mass()) {

			collision->b1->MergeBody(collision->b2);
			collision->b2->Destroy();
		}
		else {

			collision->b2->MergeBody(collision->b1);
			collision->b1->Destroy();
		}
#endif

	}

	// delete collision events
	if (collisionEvents) {

		delete collisionEvents;
		collisionEvents = nullptr;
	}


	return tasksCreated;
}

