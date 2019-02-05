
// include class header
#include "BruteForce.h"


// my class includes
#include "TaskBruteForceCPU.h"
#include "TaskIntegrateBody.h"
#include "Body.h"
#include "PhysicsUtil.h"


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
	if (MULTITHREADING) {

		farm_ = new ThreadFarm();
		farm_->SetThreadCount(NUM_OF_THREADS);
		farm_->Run();

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
	if (MULTITHREADING) {

		if (farm_) {

			farm_->End();
			delete farm_;
			farm_ = nullptr;
		}
	}
}


void BruteForce::TimeStep(float dt) {

	// Call the TimeStep method being used
	(this->*timeStepFunc_)(dt);

}

void BruteForce::TimeStepSingle(float dt) {

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

	// Loop for each body and integrate it
	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}
}


void BruteForce::TimeStepMulti(float dt) {

	// Add a BruteForceCPU task for each body to the thread farm
	for (auto body : bodies_) {

		TaskBruteForceCPU* newTask = new TaskBruteForceCPU();
		newTask->Init(body, this);
		farm_->AddTask(newTask);
	}
	
	// Wait until all the forces on each body has been calculated
	farm_->WaitUntilTasksFinished();

	// Add an integration task for each body to the thread farm
	for (auto body : bodies_) {

		TaskIntegrateBody* newTask = new TaskIntegrateBody();
		newTask->Init(body, dt);
		farm_->AddTask(newTask);
	}
	
	// Wait until all the bodies have been integrated
	farm_->WaitUntilTasksFinished();
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