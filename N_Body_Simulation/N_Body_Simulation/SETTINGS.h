#pragma once


#define WINDOW_SIZE_X		1280
#define WINDOW_SIZE_Y		720


#define ADD_RANDOM_BODIES	true
#define NUM_RAND_BODIES		1500
#define RANDOM_BODY_MAX_X	2280.0f
#define RANDOM_BODY_MAX_Y	2720.0f
#define RANDOM_BODY_MAX_Z	2720.0f
#define RANDOM_BODY_MASS	1.0E13f

#define ADD_ORBIT_BODIES	true


#define PLACE_BODY_MASS 1.0E15f

constexpr auto STEPS_PER_SECOND = 15.0f;
constexpr auto SIMULATION_SPEED = 1.0f;
#define DAMPENING_FACTOR	0.0f//0125f

constexpr auto INTERPOLATE_BETWEEN_FRAMES = true;

// methods: 0 = BruteForce, 1 = BarnesHutCPU
#define SIMULATION_METHOD 1

#define MULTITHREADING		true
#define NUM_OF_THREADS		8



#define TIMING_STEPS		true
