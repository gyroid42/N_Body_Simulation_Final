
// class header include
#include "TaskDirectCollision.h"


// my class includes
#include "BruteForce.h"
#include "Body.h"


TaskDirectCollision::TaskDirectCollision()
{
}


TaskDirectCollision::~TaskDirectCollision()
{
}


#if BENCHMARKING
void TaskDirectCollision::Init(Channel<CollisionEvent*>* newCollisionEventChannel, Channel<int>* newCheckCountChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation) {

	bodyArray_ = newBodyArray;
	simulation_ = newSimulation;
	collisionEventChannel_ = newCollisionEventChannel;
	checkCountChannel_ = newCheckCountChannel;
}


void TaskDirectCollision::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	int totalChecks = 0;
	for (auto body : *bodyArray_) {

		// check collisions
		simulation_->CheckCollision(body, collisionEvents, totalChecks);
	}

	// output collision events to events channel
	collisionEventChannel_->write(collisionEvents);
	checkCountChannel_->write(totalChecks);
}
#else
void TaskDirectCollision::Init(Channel<CollisionEvent*>* newCollisionEventChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation) {

	bodyArray_ = newBodyArray;
	simulation_ = newSimulation;
	collisionEventChannel_ = newCollisionEventChannel;
}

void TaskDirectCollision::Run() {

	// create collision events list
	CollisionEvent* collisionEvents = nullptr;

	for (auto body : *bodyArray_) {

		// check collisions
		simulation_->CheckCollision(body, collisionEvents);
	}

	// output collision events to events channel
	collisionEventChannel_->write(collisionEvents);
}
#endif
