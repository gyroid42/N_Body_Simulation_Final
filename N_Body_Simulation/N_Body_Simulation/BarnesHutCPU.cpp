
// include class header
#include "BarnesHutCPU.h"

// standard library includes
#include <iostream>

// my class includes
#include "Body.h"
#include "PartitionTree.h"
#include "TaskIntegrateBody.h"
#include "TaskInsertBody.h"
#include "TaskUpdateForces.h"


// testing
#include <chrono>

typedef std::chrono::steady_clock the_clock;


BarnesHutCPU::BarnesHutCPU() :
	farm_(nullptr)
{
}


BarnesHutCPU::~BarnesHutCPU()
{
}


void BarnesHutCPU::Init() {

	BarnesHut::Init();

	// Create the partition tree root
	root_ = Partition(sf::Vector3f(0.0f, 0.0f, 0.0f), 100000.0f);

	// If Multi-threading then start the thread farm
	// and set the TimeStep function to the multi-threaded impelmentation
	if (MULTITHREADING) {

		farm_ = new ThreadFarm();
		farm_->SetThreadCount(NUM_OF_THREADS);
		farm_->Run();

		timeStepFunc_ = &BarnesHutCPU::TimeStepMulti;
	}

	// else just set the TimeStep function to the single-threaded implementation
	else {

		timeStepFunc_ = &BarnesHutCPU::TimeStepSingle;
	}
}

void BarnesHutCPU::CleanUp() {

	BarnesHut::CleanUp();

	// If multi-threading then clean up the farm
	if (MULTITHREADING) {

		if (farm_) {

			farm_->End();
			delete farm_;
			farm_ = nullptr;
		}
	}
}


void BarnesHutCPU::TimeStep(float dt) {

	the_clock::time_point start = the_clock::now();


	// Call the TimeStep method being used
	(this->*timeStepFunc_)(dt);


	the_clock::time_point end = the_clock::now();


	//std::cout << "total time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl << std::endl;
}

void BarnesHutCPU::TimeStepSingle(float dt) {

	// Partition physics space

	// create intial tree using partition root
	PartitionTree tree(root_);

	the_clock::time_point start = the_clock::now();

	// insert each body into the partition tree
	for (auto body : bodies_) {

		tree.Insert(body);
	}

	the_clock::time_point end = the_clock::now();


	std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();


	// Calculate for acting on each body

	// loop for each body and traverse partition tree
	for (auto body : bodies_) {

		// reset body force before applying forces
		body->ResetForce();

		tree.UpdateForceOnBody(body);
	}

	end = the_clock::now();


	std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

	// loop for each body and integrate

	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}


	end = the_clock::now();


	std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
}


void BarnesHutCPU::TimeStepMulti(float dt) {

	// Partition physics space

	// create intial tree using partition root
	PartitionTree tree(root_);

	the_clock::time_point start = the_clock::now();

	// Add an Insert task for each body to farm
	for (auto body : bodies_) {

		//tree.Insert(body);

		TaskInsertBody* newTask = new TaskInsertBody();
		newTask->Init(body, &tree);
		farm_->AddTask(newTask);
	}

	// Wait until all bodies are added
	farm_->WaitUntilTasksFinished();

	the_clock::time_point end = the_clock::now();


	std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();
	// Add an UpdateForces task for each body
	for (auto body : bodies_) {

		// reset body force before applying forces
		//body->ResetForce();

		//tree.UpdateForceOnBody(body);

		TaskUpdateForces* newTask = new TaskUpdateForces();
		newTask->Init(body, &tree);
		farm_->AddTask(newTask);
	}

	// Wait until all bodies are added
	farm_->WaitUntilTasksFinished();

	end = the_clock::now();

	std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

	// Add an integration task for each body
	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);

		//TaskIntegrateBody* newTask = new TaskIntegrateBody();
		//newTask->Init(body, dt);
		//farm_->AddTask(newTask);
	}

	// Wait until all bodies are integrated
	//farm_->WaitUntilTasksFinished();

	end = the_clock::now();

	std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

}


void BarnesHutCPU::PartitionSpace() {


}


void BarnesHutCPU::CalculateForceOnBody(Body* body) {


}
