#pragma once

#include "SETTINGS.h"
#include "SettingsEnums.h"

#include <SFML/System/Vector3.hpp>


struct SimulationSettings {

	std::string simName;

	int numRandBodies;
	float maxRandX;
	float maxRandY;
	float maxRandZ;
	float randBodyMass;

	float partitionSize;

	bool collision;
	int maxCollisionDepth;

	INTEGRATION_METHOD integrationMethod;
	SIMULATION_METHOD simMethod;
	SIMULATION_MODE simMode;


	bool varyMultiThreading;
	bool multiThreading;
	int threadCount;

	bool timingSteps;

	bool varyBodies;
	int bodyCount;
	int bodyCountList[20];


	int maxSteps;
	float dt;


	float theta;



	bool orderBodies;


	int numPeriods;
	float periodLength;
	int stepsPerPeriod;

	sf::Vector3f orbitStartPos;
	sf::Vector3f orbitStartVel;
	float satelliteMass;
	float planetMass;


	SimulationSettings(SIMULATION_METHOD newSimMethod = Barnes_Hut, INTEGRATION_METHOD newIntegrationMethod = Verlet, bool newCollision = true, bool newMultiThreading = MULTITHREADING, bool newTimingSteps = true) {
		simMethod = newSimMethod;
		integrationMethod = newIntegrationMethod;
		collision = newCollision;
		multiThreading = newMultiThreading;
		timingSteps = newTimingSteps;
		numRandBodies = NUM_RAND_BODIES;
		maxRandX = RANDOM_BODY_MAX_X;
		maxRandY = RANDOM_BODY_MAX_Y;
		maxRandZ = RANDOM_BODY_MAX_Z;
		randBodyMass = RANDOM_BODY_MASS;
		partitionSize = PARTITION_SIZE;
		maxCollisionDepth = MAX_COLLISION_DEPTH;
		threadCount = NUM_OF_THREADS;
		theta = DEFAULT_THETA;

		bodyCount = NUM_RAND_BODIES;

		bodyCountList[0] = 1000;
		bodyCountList[1] = 2000;
		bodyCountList[2] = 3000;
		bodyCountList[3] = 4000;
		bodyCountList[4] = 5000;
		bodyCountList[5] = 6000;
		bodyCountList[6] = 8000;
		bodyCountList[7] = 10000;
		bodyCountList[8] = 12500;
		bodyCountList[9] = 15000;
	}
};