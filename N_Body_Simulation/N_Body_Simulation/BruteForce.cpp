#include "BruteForce.h"
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


	if (MULTITHREADING) {

		farm_ = new ThreadFarm();
		farm_->SetThreadCount(NUM_OF_THREADS);
		farm_->Run();

		timeStepFunc_ = &BruteForce::TimeStepMulti;
	}
	else {

		timeStepFunc_ = &BruteForce::TimeStepSingle;
	}

}

void BruteForce::CleanUp() {

	Simulation::CleanUp();

	if (MULTITHREADING) {

		if (farm_) {

			farm_->End();
			delete farm_;
			farm_ = nullptr;
		}
	}
}


void BruteForce::TimeStep(float dt) {


	// Evaluate each body against each other body
	/*for (auto i = bodies_.begin(); i != bodies_.end(); i++) {

		(*i)->ResetForce();
		for (auto j = bodies_.begin(); j != bodies_.end(); j++) {

			if (i != j) {

				PhysicsUtil::AddForcesBetween(*i, *j);
				//(*i)->AddForce(*j);
			}
		}

	}*/

	(this->*timeStepFunc_)(dt);

}

void BruteForce::TimeStepSingle(float dt) {

	for (auto body1 : bodies_) {

		body1->ResetForce();
		for (auto body2 : bodies_) {

			if (body1 != body2) {

				PhysicsUtil::AddForcesBetween(body1, body2);
			}
		}
	}


	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}
}


void BruteForce::TimeStepMulti(float dt) {

	for (auto body : bodies_) {

		TaskBruteForceCPU* newTask = new TaskBruteForceCPU();
		newTask->Init(body, this);
		farm_->AddTask(newTask);
	}
	
	farm_->WaitUntilTasksFinished();

	for (auto body : bodies_) {

		TaskIntegrateBody* newTask = new TaskIntegrateBody();
		newTask->Init(body, dt);
		farm_->AddTask(newTask);
	}
	
	farm_->WaitUntilTasksFinished();

	int meh = 0;
}


void BruteForce::CalculateForceOnBody(Body* body1) {


	body1->ResetForce();
	for (auto body2 : bodies_) {

		if (body1 != body2) {

			PhysicsUtil::AddForcesBetween(body1, body2);
		}
	}

}
