
// include class header
#include "BarnesHutCPU.h"

// standard library includes
#include <iostream>

// my class includes
#include "Body.h"
#include "OctreeNode.h"
#include "TaskInsertBody.h"
#include "TaskUpdateForces.h"
#include "OctreeCollision.h"
#include "TaskCollisionCheckNode.h"
#include "TaskCollisionCheckTree.h"


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
	root_ = Partition(sf::Vector3f(0.0f, 0.0f, 0.0f), PARTITION_SIZE);

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


#if TIMING_STEPS

	the_clock::time_point start = the_clock::now();

#endif

	// Call the TimeStep method being used
	(this->*timeStepFunc_)(dt);

#if TIMING_STEPS
	the_clock::time_point end = the_clock::now();

#endif
	//std::cout << "total time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl << std::endl;
}

void BarnesHutCPU::TimeStepSingle(float dt) {

	// Partition physics space

	// create intial tree using partition root
	OctreeNode tree(root_);

#if TIMING_STEPS

	the_clock::time_point start = the_clock::now();

#endif

	// insert each body into the partition tree
	for (auto body : bodies_) {

		int counter = 0;
		tree.Insert(body, counter);
	}

#if TIMING_STEPS

	the_clock::time_point end = the_clock::now();


	std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

#endif
	// Calculate for acting on each body

	// loop for each body and traverse partition tree
	for (auto body : bodies_) {

		// reset body force before applying forces
		body->ResetForce();

		tree.UpdateForceOnBody(body);
	}

#if TIMING_STEPS

	end = the_clock::now();


	std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

#endif

	// loop for each body and integrate

	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}

#if TIMING_STEPS

	end = the_clock::now();


	std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

#endif
}



void BarnesHutCPU::TimeStepMulti(float dt) {


	// Partition physics space

	// create intial tree using partition root
	OctreeNode tree(root_);

#if TIMING_STEPS

	the_clock::time_point timeStart = the_clock::now();

#endif

	//std::cout << "start" << std::endl;
	size_t length = bodies_.size() / NUM_OF_THREADS;
	size_t remain = bodies_.size() % NUM_OF_THREADS;

	size_t arrayBegin = 0;
	size_t arrayEnd = 0;

	std::vector<std::vector<Body*>*> bodyArrays;

	// Add an Insert task for each body to farm
	for (size_t bodyIndex = 0; bodyIndex < std::min((size_t)NUM_OF_THREADS, bodies_.size()); bodyIndex++) {

		arrayEnd += (remain > 0) ? (length + !!(remain--)) : length;
		//tree.Insert(body);

		//std::vector<Body*>* taskBodies_ = new std::vector<Body*>(bodies_.begin() + arrayBegin, bodies_.begin() + arrayEnd);
		bodyArrays.push_back(new std::vector<Body*>(bodies_.begin() + arrayBegin, bodies_.begin() + arrayEnd));

		arrayBegin = arrayEnd;

		TaskInsertBody* newTask = new TaskInsertBody();

		OctreeNode* threadTree = new OctreeNode(root_, &tree, 0);

		newTask->Init(&mergeTreeChannel_, threadTree, bodyArrays.at(bodyIndex));

		//newTask->Init(body, &tree);
		farm_->AddTask(newTask);
	}


	int limit = (bodies_.size() < NUM_OF_THREADS) ? bodies_.size() : NUM_OF_THREADS;


	for (int i = 0; i < limit; i++) {

		OctreeNode* mergeTree = mergeTreeChannel_.read();

		// merge the tree;
		tree.Merge(mergeTree);

		delete mergeTree;

	}


#if TIMING_STEPS

	the_clock::time_point timeEnd = the_clock::now();


	std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

#endif

	// Add an UpdateForces task for each body
	for (int i = 0; i < limit; i++) {

		// reset body force before applying forces
		//body->ResetForce();

		//tree.UpdateForceOnBody(body);

		TaskUpdateForces* newTask = new TaskUpdateForces();
		newTask->Init(bodyArrays.at(i), &tree);
		farm_->AddTask(newTask);
	}

	// Wait until all bodies are added
	farm_->WaitUntilTasksFinished();


#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

#endif

	//integrate each body
	for (auto body : bodies_) {

		std::invoke(body->Integrate, *body, dt);
	}

	// Wait until all bodies are integrated
	//farm_->WaitUntilTasksFinished();

#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

#endif

#if COLLISION


	tree.CollisionCheckParallel(farm_, bodies_.size() / NUM_OF_THREADS);



#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "Collision Test time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

#endif

#endif





	for (int i = 0; i < limit; i++) {

		delete bodyArrays.at(i);
		bodyArrays.at(i) = nullptr;
	}

	//size_t limit2 = 0;
	//length = 2;

	// clear bodieslist
	bodies_.clear();

	// get new list from partition tree
	tree.GetOrderedElementsList(bodies_);

#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "sorting time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

#endif
	
	std::cout << OctreeNode::maxListSize << std::endl;
	std::cout << OctreeNode::totalCollisions << std::endl;
}


void BarnesHutCPU::PartitionSpace() {


}


void BarnesHutCPU::CalculateForceOnBody(Body* body) {


}
