
// include class header
#include "BarnesHutCPU.h"

// standard library includes
#include <iostream>

// my class includes
#include "Body.h"
#include "OctreeNode.h"
#include "TaskIntegrateBody.h"
#include "TaskInsertBody.h"
#include "TaskUpdateForces.h"
#include "BodyChannelData.h"
#include "OctreeCollision.h"
#include "TaskCollisionCheckNode.h"
#include "TaskCollisionCheckTree.h"


// testing
#include <chrono>

typedef std::chrono::steady_clock the_clock;


int soo = 0;

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


void BarnesHutCPU::TimeStepMultiImproved(float dt) {

	OctreeNode tree(root_);

#if TIMING_STEPS

	the_clock::time_point start = the_clock::now();

#endif

	tree.CreateChildren();

	for (int i = 0; i < 8; i++) {

		//TaskInsertBodyImproved* newTask = new TaskInsertBodyImproved();
		//newTask->Init(&bodyChannels_[i], tree.GetChild(i));

		//farm_->AddTask(newTask);
	}


	for (auto body : bodies_) {

		tree.AddBody(body);

		for (int i = 0; i < 8; i++) {

			Partition newPartition = root_.GetSubDivision(i);

			if (newPartition.Contains(body->Position())) {

				BodyChannelData* newData = new BodyChannelData(body, DEFAULT);
				bodyChannels_[i].write(newData);

				break;
			}
		}
	}


	for (int i = 0; i < 8; i++) {

		BodyChannelData* newData = new BodyChannelData(nullptr, END);
		bodyChannels_[i].write(newData);
	}


	farm_->WaitUntilTasksFinished();

#if TIMING_STEPS

	the_clock::time_point end = the_clock::now();


	std::cout << "insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

#endif

	// Add an UpdateForces task for each body
	for (auto body : bodies_) {

		// reset body force before applying forces
		//body->ResetForce();

		//tree.UpdateForceOnBody(body);

		TaskUpdateForces* newTask = new TaskUpdateForces();
		//newTask->Init(body, &tree);
		farm_->AddTask(newTask);
	}

	// Wait until all bodies are added
	farm_->WaitUntilTasksFinished();


#if TIMING_STEPS

	end = the_clock::now();

	std::cout << "force time = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

	start = the_clock::now();

#endif

	// Add an integration task for each body
	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);

		//TaskIntegrateBody* newTask = new TaskIntegrateBody();
		//newTask->Init(body, dt);
		//farm_->AddTask(newTask);
	}

	// Wait until all bodies are integrated
	//farm_->WaitUntilTasksFinished();


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


#if COLLISION


	Body* ancestorList[MAX_COLLISION_DEPTH];
	ancestorList[0] = tree.GetBodyList();


	TaskCollisionCheckNode* nodeCheckTask = new TaskCollisionCheckNode();
	nodeCheckTask->Init(&tree, &collisionEventsChannel_, ancestorList);
	farm_->AddTask(nodeCheckTask);

	for (int i = 0; i < 8; i++) {

		TaskCollisionCheckTree* treeCollisionTask = new TaskCollisionCheckTree();
		treeCollisionTask->Init(tree.GetChild(i), &collisionEventsChannel_, ancestorList, 1);
		farm_->AddTask(treeCollisionTask);
	}

	CollisionEvent* collisionEvents = nullptr;

	for (int i = 0; i < NUM_OF_THREADS + 1; i++) {

		CollisionEvent* newEvents = collisionEventsChannel_.read();

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

		// handle collision here
#if COLLISION_REACTION == 0

		collision->b1->MergeBody(collision->b2);
		collision->b2->Destroy();
#endif
	}

	if (collisionEvents) {

		delete collisionEvents;
		collisionEvents = nullptr;
	}

	//farm_->WaitUntilTasksFinished();

	//tree.CollisionBegin();

	/*
	CollisionNode* collisionRoot = collisionTree_.Build(root_, 4);
	for (auto body : bodies_) {

	collisionTree_.Insert(collisionRoot, body);
	}

	#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "Collision Insert time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

	#endif

	collisionTree_.TestAllCollisions(collisionRoot);

	if (collisionRoot) {

	delete collisionRoot;
	collisionRoot = nullptr;
	}
	*/

	//for (auto b1 : bodies_) {

	//	for (auto b2 : bodies_) {

	//		if (b1 == b2) {

	//			break;
	//		}

	//		collisionTree_.TestCollision(b1, b2);

	//	}
	//}


#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "Collision Test time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

#endif

#endif


#if TIMING_STEPS

	timeEnd = the_clock::now();

	std::cout << "integrate time = " << std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() << std::endl;

	timeStart = the_clock::now();

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
