
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
#include "TaskIntegration.h"


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
	root_ = Partition(sf::Vector3f(0.0f, 0.0f, 0.0f), settings_.partitionSize);

	// If Multi-threading then start the thread farm
	// and set the TimeStep function to the multi-threaded impelmentation

	farm_ = new ThreadFarm();
	farm_->SetThreadCount(settings_.threadCount);
	farm_->Run();

	if (settings_.multiThreading) {

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
	if (farm_) {

		farm_->End();
		delete farm_;
		farm_ = nullptr;
	}
}


void BarnesHutCPU::TimeStep(float dt) {


	Simulation::TimeStep(dt);

	// Call the TimeStep method being used
	(this->*timeStepFunc_)(dt);

}

void BarnesHutCPU::TimeStepSingle(float dt) {

	the_clock::time_point start;
	the_clock::time_point end;

	// Partition physics space

	// create intial tree using partition root
	OctreeNode tree(root_);

	if (settings_.timingSteps) {

		start = the_clock::now();
	}

	// insert each body into the partition tree
	for (auto body : bodies_) {

		tree.Insert(body);
	}

	if (settings_.timingSteps) {

		end = the_clock::now();


		insertTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;

		//std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

		start = the_clock::now();

	}

	// Calculate for acting on each body

	// loop for each body and traverse partition tree
	for (auto body : bodies_) {

		// reset body force before applying forces
		body->ResetForce();

		tree.UpdateForceOnBody(body, settings_.theta);
	}

	if (settings_.timingSteps) {

		end = the_clock::now();

		forceTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;

		//std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

		start = the_clock::now();

	}

	// loop for each body and integrate

	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}

	if (settings_.timingSteps) {

		end = the_clock::now();

		integrationTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0f;

		//std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	}
}



void BarnesHutCPU::TimeStepMulti(float dt) {


#if BENCHMARKING
	Channel<int> forceCalcChannel;
	Channel<int> checkCountChannel;
#endif

	the_clock::time_point timeStart;
	the_clock::time_point timeEnd;


	// Partition physics space

	// create intial tree using partition root
	OctreeNode tree(root_);

	if (settings_.timingSteps) {


		timeStart = the_clock::now();
	}

	// Create body lists for each thread to insert
	// Also create and start each task to insert bodies

	// determine length of body list each thread uses
	size_t length = bodies_.size() / settings_.threadCount;
	size_t remain = bodies_.size() % settings_.threadCount;

	size_t arrayBegin = 0;
	size_t arrayEnd = 0;

	std::vector<std::vector<Body*>*> bodyArrays;

	// Add an Insert task for each body to farm
	for (size_t bodyIndex = 0; bodyIndex < std::min((size_t)settings_.threadCount, bodies_.size()); bodyIndex++) {

		// create array of bodies for thread to use
		arrayEnd += (remain > 0) ? (length + !!(remain--)) : length;
		bodyArrays.push_back(new std::vector<Body*>(bodies_.begin() + arrayBegin, bodies_.begin() + arrayEnd));
		arrayBegin = arrayEnd;

		// create tree used for thread to use
		OctreeNode* threadTree = new OctreeNode(root_, &tree, 0);

		// create insert task
		TaskInsertBody* newTask = new TaskInsertBody();
		newTask->Init(&mergeTreeChannel_, threadTree, bodyArrays.at(bodyIndex));
		farm_->AddTask(newTask);
	}


	// read each tree from channel created by insert tasks
	int limit = (bodies_.size() < settings_.threadCount) ? bodies_.size() : settings_.threadCount;
	for (int i = 0; i < limit; i++) {

		OctreeNode* mergeTree = mergeTreeChannel_.read();

		if (mergeTree) {
			// merge the tree;
			tree.Merge(mergeTree);
		}

		delete mergeTree;
	}


	if (settings_.timingSteps) {

		timeEnd = the_clock::now();

#if BENCHMARKING

		insertTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count());
#endif

		insertTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / 1000.0f;

		//std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

		timeStart = the_clock::now();

	}





	// Add an UpdateForces task for each body
	for (int i = 0; i < limit; i++) {

		TaskUpdateForces* newTask = new TaskUpdateForces();

#if BENCHMARKING
		newTask->Init(&forceCalcChannel, bodyArrays.at(i), &tree, settings_.theta);
#else
		newTask->Init(bodyArrays.at(i), &tree, settings_.theta);
#endif
		farm_->AddTask(newTask);
	}

	// Wait until all bodies are added
	farm_->WaitUntilTasksFinished();


	if (settings_.timingSteps) {

		timeEnd = the_clock::now();

#if BENCHMARKING

		forceCalcTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count());

#endif

		forceTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / 1000.0f;

		//std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

		timeStart = the_clock::now();

	}


#if BENCHMARKING

	int totalForceCalcs = 0;


	// for each task created
	for (int i = 0; i < bodyArrays.size(); i++) {

		// add number of force calculations performed in task to total
		totalForceCalcs += forceCalcChannel.read();
	}

	numForceCalculations_.push_back(totalForceCalcs);

	timeStart = the_clock::now();

#endif



	//integrate each body
	for (int i = 0; i < bodyArrays.size(); i++) {

		TaskIntegration* newTask = new TaskIntegration();
		newTask->Init(bodyArrays.at(i), dt);
		farm_->AddTask(newTask);
	}


	farm_->WaitUntilTasksFinished();


	if (settings_.timingSteps) {

		timeEnd = the_clock::now();
#if BENCHMARKING

		integrationTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count());

#endif

		integrationTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / 1000.0f;

		//std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

		timeStart = the_clock::now();

	}

	if (settings_.collision) {

		// Detect collision using parallel algorithm

#if BENCHMARKING
		int collisionTasksCreated = tree.CollisionCheckParallel(farm_, bodies_.size() / settings_.threadCount, &checkCountChannel);
#else
		tree.CollisionCheckParallel(farm_, bodies_.size() / settings_.threadCount);
#endif


		if (settings_.timingSteps) {

			timeEnd = the_clock::now();

#if BENCHMARKING

			collisionCheckTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count());

#endif
			collisionTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / 1000.0f;

			//std::cout << "Collision Test time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

			timeStart = the_clock::now();

		}


#if BENCHMARKING
		int totalChecks = 0;


		// for each task created
		for (int i = 0; i < collisionTasksCreated; i++) {

			// add number of force calculations performed in task to total
			totalChecks += checkCountChannel.read();
		}

		numCollisionChecks_.push_back(totalChecks);

		timeStart = the_clock::now();
#endif

	}



	// delete body arrays used for insertion and force calculations
	for (int i = 0; i < limit; i++) {

		delete bodyArrays.at(i);
		bodyArrays.at(i) = nullptr;
	}


	if (settings_.orderBodies) {
		// Construct new ordered bodies list

		std::unique_lock<std::mutex> lock(bodyListMutex_);

		// clear bodieslist
		bodies_.clear();

		// get new list from partition tree
		tree.GetOrderedElementsList(bodies_);

		if (settings_.timingSteps) {

			timeEnd = the_clock::now();

#if BENCHMARKING

			sortTimes_.push_back(std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count());

#endif
			sortTime_ = (float)std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - timeStart).count() / 1000.0f;

			//std::cout << "sorting time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

		}
	}
	
	//std::cout << OctreeNode::maxListSize << std::endl;
	//std::cout << OctreeNode::totalCollisions << std::endl;
}


void BarnesHutCPU::PartitionSpace() {


}


void BarnesHutCPU::CalculateForceOnBody(Body* body) {


}
