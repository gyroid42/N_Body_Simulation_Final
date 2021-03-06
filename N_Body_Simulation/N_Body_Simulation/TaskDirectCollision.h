#pragma once

// include parent class
#include "Task.h"

// standard library include
#include <vector>

// include my classes
#include "Channel.h"
#include "SETTINGS.h"

// forward declarations
class BruteForce;
class Body;
struct CollisionEvent;


class TaskDirectCollision :
	public Task
{
public:
	TaskDirectCollision();
	~TaskDirectCollision();

#if BENCHMARKING
	void Init(Channel<CollisionEvent*>* newCollisionEventChannel, Channel<int>* newCheckCountChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation);
#else
	void Init(Channel<CollisionEvent*>* newCollisionEventChannel, std::vector<Body*>* newBodyArray, BruteForce* newSimulation);
#endif

	void Run();

private:

	// references to body and simulation
	std::vector<Body*>* bodyArray_;
	BruteForce* simulation_;

	// channel to output collisions found
	Channel<CollisionEvent*>* collisionEventChannel_;

#if BENCHMARKING
	Channel<int>* checkCountChannel_;
#endif
};

