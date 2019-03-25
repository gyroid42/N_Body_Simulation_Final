#pragma once

#include "SETTINGS.h"
#include "SettingsEnums.h"


struct SimulationSettings {

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
	int bodyCount[20];


	int maxSteps;
	float dt;


	// what to measure
	bool dataTimeSteps;
	bool dataForceCalculations;
	bool dataCollisionChecks;


	SimulationSettings(SIMULATION_METHOD newSimMethod = Barnes_Hut, INTEGRATION_METHOD newIntegrationMethod = Verlet, bool newCollision = true, bool newMultiThreading = true, bool newTimingSteps = true) {
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

		bodyCount[0] = 100;
		bodyCount[1] = 200;
		bodyCount[2] = 400;
		bodyCount[3] = 600;
		bodyCount[4] = 800;
		bodyCount[5] = 1000;
		bodyCount[6] = 1500;
		bodyCount[7] = 2000;
		bodyCount[8] = 2500;
		bodyCount[9] = 3000;
		bodyCount[10] = 4000;
		bodyCount[11] = 5000;
		bodyCount[12] = 6000;
		bodyCount[13] = 8000;
		bodyCount[14] = 10000;
		bodyCount[15] = 12000;
		bodyCount[16] = 14000;
		bodyCount[17] = 16000;
		bodyCount[18] = 18000;
		bodyCount[19] = 20000;
	}
};