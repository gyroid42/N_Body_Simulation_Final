
// include class header
#include "BruteForce.h"


// my class includes
#include "TaskBruteForceCPU.h"
#include "TaskDirectCollision.h"
#include "Body.h"
#include "PhysicsUtil.h"
#include "Channel.h"
#include "CollisionEvent.h"
#include "TaskIntegration.h"


// testing
#include <chrono>
#include <iostream>

typedef std::chrono::steady_clock the_clock;


BruteForce::BruteForce() :
	farm_(nullptr)
{
}


BruteForce::~BruteForce()
{
}


void BruteForce::Init() {

	Simulation::Init();


	// If Multithreading then start the thread farm
	// and set the TimeStep function to the multithreaded implementation

	farm_ = new ThreadFarm();
	farm_->SetThreadCount(settings_.threadCount);
	farm_->Run();


	if (settings_.multiThreading) {

		timeStepFunc_ = &BruteForce::TimeStepMulti;
	}

	// else just set the TimeStep functio nto the singlethreaded implementation
	else {

		timeStepFunc_ = &BruteForce::TimeStepSingle;
	}

}

void BruteForce::CleanUp() {

	Simulation::CleanUp();

	// If multithreading then clean up the farm

	if (farm_) {

		farm_->End();
		delete farm_;
		farm_ = nullptr;
	}
}


void BruteForce::TimeStep(float dt) {



	// Call the TimeStep method being used
	(this->*timeStepFunc_)(dt);

}

void BruteForce::TimeStepSingle(float dt) {

	the_clock::time_point start;
	the_clock::time_point end;

	if (settings_.timingSteps) {

		start = the_clock::now();
	}

	// Loop for each body
	for (auto body1 : bodies_) {

		// Reset the force on the body
		body1->ResetForce();

		// Calculate and add the forces scting on the body due to every other body
		for (auto body2 : bodies_) {

			if (body1 != body2) {

				PhysicsUtil::AddForcesBetween(body1, body2);
			}
		}
	}

	if (settings_.timingSteps) {


		end = the_clock::now();

		//std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

		start = the_clock::now();
	}

	// Loop for each body and integrate it
	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}


	if (settings_.timingSteps) {

		end = the_clock::now();

		//std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

		start = the_clock::now();
	}

	if (settings_.collision) {

		for (auto b1 : bodies_) {

			for (auto b2 : bodies_) {

				if (b1 == b2) {

					break;
				}

				collisionTree_.TestCollision(b1, b2);

			}
		}


		if (settings_.timingSteps) {


			end = the_clock::now();

			//std::cout << "Collision Test time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;


		}
	}
}


void BruteForce::TimeStepMulti(float dt) {


	the_clock::time_point end;
	the_clock::time_point start;

	if (settings_.timingSteps) {

		start = the_clock::now();
	}

	// create body lists for thread tasks

	size_t length = bodies_.size() / settings_.threadCount;
	size_t remain = bodies_.size() % settings_.threadCount;

	size_t arrayBegin = 0;
	size_t arrayEnd = 0;

	std::vector<std::vector<Body*>*> bodyArrays;

	for (size_t bodyIndex = 0; bodyIndex < std::min((size_t)settings_.threadCount, bodies_.size()); bodyIndex++) {

		arrayEnd += (remain > 0) ? (length + !!(remain--)) : length;
		bodyArrays.push_back(new std::vector<Body*>(bodies_.begin() + arrayBegin, bodies_.begin() + arrayEnd));
		arrayBegin = arrayEnd;
	}

#if BENCHMARKING
	Channel<int> calcCountChannel;
#endif

	// Add a BruteForceCPU task for each body to the thread farm
	for (int i = 0; i < bodyArrays.size(); i++) {

		TaskBruteForceCPU* newTask = new TaskBruteForceCPU();
#if BENCHMARKING
		newTask->Init(&calcCountChannel, bodyArrays.at(i), this);
#else
		newTask->Init(bodyArrays.at(i), this);
#endif
		farm_->AddTask(newTask);
	}
	
	// Wait until all the forces on each body has been calculated
	farm_->WaitUntilTasksFinished();


	if (settings_.timingSteps) {


		end = the_clock::now();

#if BENCHMARKING

		forceCalcTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
#endif
		//std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

		start = the_clock::now();
	}

#if BENCHMARKING
	int totalForceCalcs = 0;


	// for each task created
	for (int i = 0; i < bodyArrays.size(); i++) {

		// add number of force calculations performed in task to total
		totalForceCalcs += calcCountChannel.read();
	}

	numForceCalculations_.push_back(totalForceCalcs);

	start = the_clock::now();
#endif


	// Add an integration task for each body to the thread farm
	//integrate each body
	for (int i = 0; i < bodyArrays.size(); i++) {

		TaskIntegration* newTask = new TaskIntegration();
		newTask->Init(bodyArrays.at(i), dt);
		farm_->AddTask(newTask);
	}


	farm_->WaitUntilTasksFinished();


	if (settings_.timingSteps) {


		end = the_clock::now();

#if BENCHMARKING

		integrationTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
#endif
		//std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	}

	if (settings_.collision) {

		
		CheckAllCollisions(&bodyArrays);


	}
}


void BruteForce::CalculateForceOnBody(Body* body1) {

	// Reset the force on the body
	body1->ResetForce();

	// Loop for all the other bodies and the force due to them
	for (auto body2 : bodies_) {

		if (body1 != body2) {

			PhysicsUtil::AddForcesBetween(body1, body2);
		}
	}
}

void BruteForce::CalculateForceOnBody(Body* body1, int& totalForceCalcs) {

	// Reset the force on the body
	body1->ResetForce();

	// Loop for all the other bodies and the force due to them
	for (auto body2 : bodies_) {

		if (body1 != body2) {

			PhysicsUtil::AddForcesBetween(body1, body2);
		}
	}

	totalForceCalcs += bodies_.size() - 1;
}



void BruteForce::CheckCollision(Body* b1, CollisionEvent*& collisionEvents) {


	for (auto b2 : bodies_) {

		// don't check collision if bodies are the same
		if (b1 == b2) {

			break;
		}

		// Check collision using bounding spheres
		if (SphereToSphereCollision(b1, b2)) {

			// collision has happened so create a collision event
			CollisionEvent* newCollision = new CollisionEvent(b1, b2);

			if (collisionEvents) {

				newCollision->next = collisionEvents;
			}

			collisionEvents = newCollision;
		}
	}
}

void BruteForce::CheckCollision(Body* b1, CollisionEvent*& collisionEvents, int& totalChecks) {


	for (auto b2 : bodies_) {

		// don't check collision if bodies are the same
		if (b1 == b2) {

			break;
		}

		totalChecks++;

		// Check collision using bounding spheres
		if (SphereToSphereCollision(b1, b2)) {

			// collision has happened so create a collision event
			CollisionEvent* newCollision = new CollisionEvent(b1, b2);

			if (collisionEvents) {

				newCollision->next = collisionEvents;
			}

			collisionEvents = newCollision;
		}
	}
}


bool BruteForce::SphereToSphereCollision(Body* b1, Body* b2) {

	// x1*x2 + y1*y2 + z1*z2 <= (r1+r2)*(r1*r2)

	float distance = PhysicsUtil::DistanceToSqr(b1->Position(), b2->Position());
	float radius = b1->ModelRadius() + b2->ModelRadius();

	return distance <= radius * radius;
}


void BruteForce::CheckAllCollisions(std::vector<std::vector<Body*>*>* bodyArrays) {

	the_clock::time_point end;
	the_clock::time_point start;

#if BENCHMARKING
	Channel<int> checkCountChannel;
#endif

	if (settings_.timingSteps) {

		start = the_clock::now();
	}

	Channel<CollisionEvent*> collisionEventChannel;

	for (int i = 0; i < bodyArrays->size(); i++) {

		TaskDirectCollision* newTask = new TaskDirectCollision();

#if BENCHMARKING
		newTask->Init(&collisionEventChannel, &checkCountChannel, bodyArrays->at(i), this);
#else
		newTask->Init(&collisionEventChannel, bodyArrays->at(i), this);
#endif
		farm_->AddTask(newTask);
	}

	CollisionEvent* collisionEvents = nullptr;

	// for each task created
	for (int i = 0; i < bodyArrays->size(); i++) {

		// get collision events created
		CollisionEvent* newEvents = collisionEventChannel.read();

		if (newEvents) {

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

	for (CollisionEvent* collision = collisionEvents; collision; collision = collision->next) {

#if COLLISION_REACTION == 0

		// Collision reaction 0 is to merge the bodies in the collision

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


	if (settings_.timingSteps) {

		end = the_clock::now();

#if BENCHMARKING

		collisionCheckTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
#endif
		//std::cout << "collision time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
	}

#if BENCHMARKING
	int totalCollisionChecks = 0;


	// for each task created
	for (int i = 0; i < bodyArrays->size(); i++) {

		// get collision events created
		totalCollisionChecks += checkCountChannel.read();
	}

	numCollisionChecks_.push_back(totalCollisionChecks);
#endif
}
