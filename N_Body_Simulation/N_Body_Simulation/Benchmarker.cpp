#include "Benchmarker.h"


// std library includes
#include <iostream>
#include <conio.h>

// My class includes
#include "Simulation.h"
#include "Input.h"
#include "BarnesHutCPU.h"
#include "BruteForce.h"
#include "SortVector.h"


Benchmarker::Benchmarker() :
	input_(nullptr),
	simulation_(nullptr),
	outputBook_(nullptr)
{
}


Benchmarker::~Benchmarker()
{

	if (input_) {

		delete input_;
		input_ = nullptr;
	}

	EndSimulation();

	if (outputBook_) {

		outputBook_->release();
		outputBook_ = nullptr;
	}
}





void Benchmarker::Init(Input* newInput) {


	std::cout << "Initialising Benchmarker..." << std::endl;

	input_ = newInput;

	outputBook_ = nullptr;

	std::cout << "Creating test scenarios..." << std::endl;

	CreateSimulationSettings();

	std::cout << "Finished creating test scenarios" << std::endl;

	std::cout << "Finished initialising" << std::endl;
}


void Benchmarker::CreateSimulationSettings() {

	// varying integration


	SimulationSettings varyIntegration1;

	varyIntegration1.simName = "SemiEuler_Direct";

	varyIntegration1.collision = false;

	varyIntegration1.integrationMethod = Semi_Implicit_Euler;

	varyIntegration1.simMethod = Direct;
	varyIntegration1.theta = 1.0f;
	varyIntegration1.orderBodies = true;

	varyIntegration1.simMode = Even_Distribution;
	varyIntegration1.varyBodies = true;
	varyIntegration1.bodyCount = 2000;

	varyIntegration1.dt = 1.0f / 15.0f;


	varyIntegration1.threadCount = 2;
	varyIntegration1.multiThreading = true;
	varyIntegration1.varyMultiThreading = true;

	varyIntegration1.timingSteps = true;

	
	varyIntegration1.maxSteps = 100;
	varyIntegration1.partitionSize = 10000.0f;


	//benchmarkSettingsList_.push_back(varyIntegration1);

	varyIntegration1.threadCount = 1;


	SimulationSettings varyIntegration4 = varyIntegration1;
	varyIntegration4.simName = "SemiEuler_Barnes";
	varyIntegration4.integrationMethod = Semi_Implicit_Euler;
	varyIntegration4.simMethod = Barnes_Hut;

	//benchmarkSettingsList_.push_back(varyIntegration4);

	SimulationSettings varyIntegration5 = varyIntegration1;
	varyIntegration5.simName = "ExplicitEuler_Barnes";
	varyIntegration5.integrationMethod = Explicit_Euler;
	varyIntegration5.simMethod = Barnes_Hut;

	//benchmarkSettingsList_.push_back(varyIntegration5);

	SimulationSettings varyIntegration6 = varyIntegration1;
	varyIntegration6.simName = "Verlet_Barnes";
	varyIntegration6.integrationMethod = Verlet;
	varyIntegration6.simMethod = Barnes_Hut;

	//benchmarkSettingsList_.push_back(varyIntegration6);

	// vary theta and distribution
	int i = 5;
	for (float newTheta = 1.0f; newTheta <= 2.0f; newTheta += 0.2f) {

		SimulationSettings varyTheta1 = varyIntegration1;
		varyTheta1.simName = "EvenDis_Theta";
		varyTheta1.simName += std::to_string(i);
		varyTheta1.simMethod = Barnes_Hut;
		varyTheta1.theta = newTheta;
		varyTheta1.simMode = Even_Distribution;

		//benchmarkSettingsList_.push_back(varyTheta1);

		i++;
	}

	// vary theta and distribution
	i = 0;
	for (float newTheta = 0.6f; newTheta <= 2.0f; newTheta += 0.2f) {

		SimulationSettings varyTheta1 = varyIntegration1;
		varyTheta1.simName = "ClusterDis_Theta";
		varyTheta1.simName += std::to_string(i);
		varyTheta1.simMethod = Barnes_Hut;
		varyTheta1.theta = newTheta;
		varyTheta1.simMode = Clustered_Distribution;

		//benchmarkSettingsList_.push_back(varyTheta1);

		i++;
	}


	SimulationSettings noSort = varyIntegration1;
	noSort.simName = "No_Sort";
	noSort.simMethod = Barnes_Hut;
	noSort.orderBodies = false;

	benchmarkSettingsList_.push_back(noSort);


	// collision stuffs
	SimulationSettings collisionDirect = varyIntegration1;
	collisionDirect.simName = "CollisionDirect";
	collisionDirect.collision = true;
	collisionDirect.simMode = Even_Distribution;

	benchmarkSettingsList_.push_back(collisionDirect);

	SimulationSettings collisionBarnes1 = varyIntegration1;
	collisionBarnes1.simName = "Collision_Barnes_Even";
	collisionBarnes1.collision = true;
	collisionBarnes1.simMethod = Barnes_Hut;
	collisionBarnes1.simMode = Even_Distribution;

	benchmarkSettingsList_.push_back(collisionBarnes1);

	SimulationSettings collisionBarnes2 = varyIntegration1;
	collisionBarnes2.simName = "Collision_Barnes_Cluster";
	collisionBarnes2.collision = true;
	collisionBarnes2.simMethod = Barnes_Hut;
	collisionBarnes2.simMode = Clustered_Distribution;

	benchmarkSettingsList_.push_back(collisionBarnes2);

	

	//benchmarkSettingsList_.push_back(test2);
}



void Benchmarker::MainLoop() {


	std::cout << std::endl << std::endl << "Starting Benchmarking" << std::endl;

	// run a simulation for each setting
	for (auto currentSettings : benchmarkSettingsList_) {

		outputBook_ = xlCreateBook();

		std::cout << std::endl << "running " << currentSettings.simName << " test..." << std::endl;

		libxl::Sheet* summarySheet = nullptr;

		if (outputBook_) {

			summarySheet = outputBook_->addSheet(std::string(currentSettings.simName + " Summary").c_str());

			if (summarySheet) {

				// write settings list
				summarySheet->writeStr(6, 2, "Settings");

				summarySheet->writeStr(8, 2, "Method");
				switch (currentSettings.simMethod) {
				case Direct:
					summarySheet->writeStr(8, 3, "Direct");
					break;
				case Barnes_Hut:
					summarySheet->writeStr(8, 3, "Barnes-Hut");
					break;
				default:
					summarySheet->writeStr(8, 3, "Error");
					break;
				}


				summarySheet->writeStr(9, 2, "Mode");
				switch (currentSettings.simMode) {
				case Random_Bodies:
					summarySheet->writeStr(9, 3, "Random");
					break;
				case Two_Body_Orbit:
					summarySheet->writeStr(9, 3, "Two_Body_Orbit");
					break;
				case Even_Distribution:
					summarySheet->writeStr(9, 3, "Even Distribution");
					break;
				case Clustered_Distribution:
					summarySheet->writeStr(9, 3, "Clustered Distribution");
					break;
				case Asteroids:
					summarySheet->writeStr(9, 3, "Asteroids");
					break;
				default:
					summarySheet->writeStr(9, 3, "Error");
					break;
				}

				summarySheet->writeStr(10, 2, "Integration");
				switch (currentSettings.integrationMethod) {
				case Semi_Implicit_Euler:
					summarySheet->writeStr(10, 3, "Semi-Implicit Euler");
					break;
				case Explicit_Euler:
					summarySheet->writeStr(10, 3, "Explicit Euler");
					break;
				case Verlet:
					summarySheet->writeStr(10, 3, "Verlet");
					break;
				case Runge_Kutta:
					summarySheet->writeStr(10, 3, "4th Order Runge-Kutta");
					break;
				default:
					summarySheet->writeStr(10, 3, "Error");
					break;
				}

				summarySheet->writeStr(12, 2, "Total Steps");
				summarySheet->writeNum(12, 3, currentSettings.maxSteps);


				summarySheet->writeStr(14, 2, "Threads");
				summarySheet->writeNum(14, 3, currentSettings.threadCount);

				summarySheet->writeStr(15, 2, "Collision");
				summarySheet->writeBool(15, 3, currentSettings.collision);

				summarySheet->writeStr(16, 2, "Time Step");
				summarySheet->writeNum(16, 3, currentSettings.dt);

				summarySheet->writeStr(17, 2, "Theta");
				summarySheet->writeNum(17, 3, currentSettings.theta);
			}
		}

		int threadIndex = 0;

		// loop for each thread count in range (1, 2, 4, 6, 8, 10, 12...)
		for (; currentSettings.threadCount <= std::thread::hardware_concurrency(); (currentSettings.threadCount % 2) ? currentSettings.threadCount++ : currentSettings.threadCount += 2) {


			std::string testName = currentSettings.simName
				+ " T_" + std::to_string(currentSettings.threadCount);

			libxl::Sheet* currentSheet = nullptr;

			if (outputBook_) {

				currentSheet = outputBook_->addSheet(testName.c_str());

				if (currentSheet) {

					// write settings list
					currentSheet->writeStr(6, 2, "Settings");

					currentSheet->writeStr(8, 2, "Method");
					switch (currentSettings.simMethod) {
					case Direct:
						currentSheet->writeStr(8, 3, "Direct");
						break;
					case Barnes_Hut:
						currentSheet->writeStr(8, 3, "Barnes-Hut");
						break;
					default:
						currentSheet->writeStr(8, 3, "Error");
						break;
					}


					currentSheet->writeStr(9, 2, "Mode");
					switch (currentSettings.simMode) {
					case Random_Bodies:
						currentSheet->writeStr(9, 3, "Random");
						break;
					case Two_Body_Orbit:
						currentSheet->writeStr(9, 3, "Two_Body_Orbit");
						break;
					case Even_Distribution:
						currentSheet->writeStr(9, 3, "Even Distribution");
						break;
					case Clustered_Distribution:
						currentSheet->writeStr(9, 3, "Clustered Distribution");
						break;
					case Asteroids:
						currentSheet->writeStr(9, 3, "Asteroids");
						break;
					default:
						currentSheet->writeStr(9, 3, "Error");
						break;
					}

					currentSheet->writeStr(10, 2, "Integration");
					switch (currentSettings.integrationMethod) {
					case Semi_Implicit_Euler:
						currentSheet->writeStr(10, 3, "Semi-Implicit Euler");
						break;
					case Explicit_Euler:
						currentSheet->writeStr(10, 3, "Explicit Euler");
						break;
					case Verlet:
						currentSheet->writeStr(10, 3, "Verlet");
						break;
					case Runge_Kutta:
						currentSheet->writeStr(10, 3, "4th Order Runge-Kutta");
						break;
					default:
						currentSheet->writeStr(10, 3, "Error");
						break;
					}

					currentSheet->writeStr(12, 2, "Total Steps");
					currentSheet->writeNum(12, 3, currentSettings.maxSteps);


					currentSheet->writeStr(14, 2, "Threads");
					currentSheet->writeNum(14, 3, currentSettings.threadCount);

					currentSheet->writeStr(15, 2, "Collision");
					currentSheet->writeBool(15, 3, currentSettings.collision);

					currentSheet->writeStr(16, 2, "Time Step");
					currentSheet->writeNum(16, 3, currentSettings.dt);

					currentSheet->writeStr(17, 2, "Theta");
					currentSheet->writeNum(17, 3, currentSettings.theta);
				}
			}

			// loop for each body count in body count list
			for (int bodyCountIndex = 0; bodyCountIndex < 10; bodyCountIndex++) {

				// update body count for current simulation
				if (currentSettings.varyBodies) {

					currentSettings.bodyCount = currentSettings.bodyCountList[bodyCountIndex];
				}

				std::cout << "running for " << currentSettings.bodyCount << " bodies with " << currentSettings.threadCount << " threads..." << std::endl;

				
				StartSimulation(&currentSettings);


				// do a timestep until max has been hit
				for (int i = 0; i < currentSettings.maxSteps; i++) {

					simulation_->TimeStep(currentSettings.dt);
				}


				std::cout << "finished test" << std::endl;

				// write results into excel spreadsheet

				std::cout << "writing results to output file..." << std::endl;

				if (outputBook_) {


					int forceMedian = 0, collisionMedian = 0, integrationMedian = 0, insertMedian = 0, sortMedian = 0;
					int numForceCalcMedian = 0, numCollisionCheckMedian = 0;

					if (currentSheet) {




						currentSheet->writeStr(4, 5 + bodyCountIndex * 11, "Num Bodies");
						currentSheet->writeNum(5, 5 + bodyCountIndex * 11, currentSettings.bodyCount);


						currentSheet->writeStr(9, 5 + bodyCountIndex * 11, "Lowest");
						currentSheet->writeStr(10, 5 + bodyCountIndex * 11, "Q1");
						currentSheet->writeStr(11, 5 + bodyCountIndex * 11, "Median");
						currentSheet->writeStr(12, 5 + bodyCountIndex * 11, "Q3");
						currentSheet->writeStr(13, 5 + bodyCountIndex * 11, "Highest");


						// Label table

						// time step count
						currentSheet->writeStr(15, 5 + bodyCountIndex * 11, "Time Step");
						for (int i = 0; i < currentSettings.maxSteps; i++) {

							currentSheet->writeNum(16 + i, 5 + bodyCountIndex * 11, i);
						}

#if BENCHMARKING

						// Timing labels
						currentSheet->writeStr(6, 6 + bodyCountIndex * 11, "Timing");


						// FORCE CALC TIME

						// raw data
						currentSheet->writeStr(15, 6 + bodyCountIndex * 11, "Force Time");

						std::vector<int> forceCalcTimes = simulation_->GetForceCalcTimes();
						for (int i = 0; i < forceCalcTimes.size(); i++) {

							currentSheet->writeNum(16 + i, 6 + bodyCountIndex * 11, forceCalcTimes.at(i));
						}

						// averages
						currentSheet->writeStr(8, 6 + bodyCountIndex * 11, "Force Time");

						std::vector<int> orderedForceCalcTimes = Sort(forceCalcTimes);
						currentSheet->writeNum(9, 6 + bodyCountIndex * 11, orderedForceCalcTimes.at(0));
						currentSheet->writeNum(10, 6 + bodyCountIndex * 11, orderedForceCalcTimes.at(orderedForceCalcTimes.size() / 4));
						currentSheet->writeNum(11, 6 + bodyCountIndex * 11, orderedForceCalcTimes.at(orderedForceCalcTimes.size() / 2));
						currentSheet->writeNum(12, 6 + bodyCountIndex * 11, orderedForceCalcTimes.at((orderedForceCalcTimes.size() * 3) / 4));
						currentSheet->writeNum(13, 6 + bodyCountIndex * 11, orderedForceCalcTimes.at(orderedForceCalcTimes.size() - 1));

						forceMedian = orderedForceCalcTimes.at(orderedForceCalcTimes.size() / 2);


						if (currentSettings.collision) {
							// COLLISION CHECK TIME

							// raw data
							currentSheet->writeStr(15, 7 + bodyCountIndex * 11, "Collision Check Time");

							std::vector<int> collisionCheckTimes = simulation_->GetCollisionCheckTimes();
							for (int i = 0; i < collisionCheckTimes.size(); i++) {

								currentSheet->writeNum(16 + i, 7 + bodyCountIndex * 11, collisionCheckTimes.at(i));
							}

							// averages
							currentSheet->writeStr(8, 7 + bodyCountIndex * 11, "Collision Check Time");

							std::vector<int> orderedCollisionCheckTimes = Sort(collisionCheckTimes);
							currentSheet->writeNum(9, 7 + bodyCountIndex * 11, orderedCollisionCheckTimes.at(0));
							currentSheet->writeNum(10, 7 + bodyCountIndex * 11, orderedCollisionCheckTimes.at(orderedCollisionCheckTimes.size() / 4));
							currentSheet->writeNum(11, 7 + bodyCountIndex * 11, orderedCollisionCheckTimes.at(orderedCollisionCheckTimes.size() / 2));
							currentSheet->writeNum(12, 7 + bodyCountIndex * 11, orderedCollisionCheckTimes.at((orderedCollisionCheckTimes.size() * 3) / 4));
							currentSheet->writeNum(13, 7 + bodyCountIndex * 11, orderedCollisionCheckTimes.at(orderedCollisionCheckTimes.size() - 1));


							collisionMedian = orderedCollisionCheckTimes.at(orderedCollisionCheckTimes.size() / 2);

						}
						// INTEGRATION TIME

						// raw data
						currentSheet->writeStr(15, 8 + bodyCountIndex * 11, "Integration Time");

						std::vector<int> integrationTimes = simulation_->GetIntegrationTimes();
						for (int i = 0; i < integrationTimes.size(); i++) {

							currentSheet->writeNum(16 + i, 8 + bodyCountIndex * 11, integrationTimes.at(i));
						}


						// averages
						currentSheet->writeStr(8, 8 + bodyCountIndex * 11, "Integration Time");

						std::vector<int> orderedIntegrationTimes = Sort(integrationTimes);
						currentSheet->writeNum(9, 8 + bodyCountIndex * 11, orderedIntegrationTimes.at(0));
						currentSheet->writeNum(10, 8 + bodyCountIndex * 11, orderedIntegrationTimes.at(orderedIntegrationTimes.size() / 4));
						currentSheet->writeNum(11, 8 + bodyCountIndex * 11, orderedIntegrationTimes.at(orderedIntegrationTimes.size() / 2));
						currentSheet->writeNum(12, 8 + bodyCountIndex * 11, orderedIntegrationTimes.at((orderedIntegrationTimes.size() * 3) / 4));
						currentSheet->writeNum(13, 8 + bodyCountIndex * 11, orderedIntegrationTimes.at(orderedIntegrationTimes.size() - 1));

						integrationMedian = orderedIntegrationTimes.at(orderedIntegrationTimes.size() / 2);


						if (currentSettings.simMethod == Barnes_Hut) {


							// INSERT TIME

							// raw data
							currentSheet->writeStr(15, 9 + bodyCountIndex * 11, "Insert Time");

							std::vector<int> insertTimes = ((BarnesHutCPU*)simulation_)->GetInsertTimes();
							for (int i = 0; i < insertTimes.size(); i++) {

								currentSheet->writeNum(16 + i, 9 + bodyCountIndex * 11, insertTimes.at(i));
							}


							// averages
							currentSheet->writeStr(8, 9 + bodyCountIndex * 11, "Insert Time");

							std::vector<int> orderedInsertTimes = Sort(insertTimes);
							currentSheet->writeNum(9, 9 + bodyCountIndex * 11, orderedInsertTimes.at(0));
							currentSheet->writeNum(10, 9 + bodyCountIndex * 11, orderedInsertTimes.at(orderedInsertTimes.size() / 4));
							currentSheet->writeNum(11, 9 + bodyCountIndex * 11, orderedInsertTimes.at(orderedInsertTimes.size() / 2));
							currentSheet->writeNum(12, 9 + bodyCountIndex * 11, orderedInsertTimes.at((orderedInsertTimes.size() * 3) / 4));
							currentSheet->writeNum(13, 9 + bodyCountIndex * 11, orderedInsertTimes.at(orderedInsertTimes.size() - 1));


							insertMedian = orderedInsertTimes.at(orderedInsertTimes.size() / 2);



							if (currentSettings.orderBodies) {
								// SORT TIME

								// raw data
								currentSheet->writeStr(15, 10 + bodyCountIndex * 11, "Sort Time");

								std::vector<int> sortTimes = ((BarnesHutCPU*)simulation_)->GetSortTimes();
								for (int i = 0; i < sortTimes.size(); i++) {

									currentSheet->writeNum(16 + i, 10 + bodyCountIndex * 11, sortTimes.at(i));
								}


								// averages
								currentSheet->writeStr(8, 10 + bodyCountIndex * 11, "Sort Time");

								std::vector<int> orderedSortTimes = Sort(sortTimes);
								currentSheet->writeNum(9, 10 + bodyCountIndex * 11, orderedSortTimes.at(0));
								currentSheet->writeNum(10, 10 + bodyCountIndex * 11, orderedSortTimes.at(orderedSortTimes.size() / 4));
								currentSheet->writeNum(11, 10 + bodyCountIndex * 11, orderedSortTimes.at(orderedSortTimes.size() / 2));
								currentSheet->writeNum(12, 10 + bodyCountIndex * 11, orderedSortTimes.at((orderedSortTimes.size() * 3) / 4));
								currentSheet->writeNum(13, 10 + bodyCountIndex * 11, orderedSortTimes.at(orderedSortTimes.size() - 1));


								sortMedian = orderedSortTimes.at(orderedSortTimes.size() / 2);
							}
						}



						currentSheet->writeStr(6, 12 + bodyCountIndex * 11, "Num Operations");


						// NUM OF FORCE CALCS

						// raw data
						currentSheet->writeStr(15, 12 + bodyCountIndex * 11, "Num Force Calculations");
						std::vector<int> numForceCalcs = simulation_->GetNumForceCalcs();
						for (int i = 0; i < numForceCalcs.size(); i++) {

							currentSheet->writeNum(16 + i, 12 + bodyCountIndex * 11, numForceCalcs.at(i));
						}


						// averages
						currentSheet->writeStr(8, 12 + bodyCountIndex * 11, "Num Force Calculations");

						std::vector<int> orderedNumForceCalcs = Sort(numForceCalcs);
						currentSheet->writeNum(9, 12 + bodyCountIndex * 11, orderedNumForceCalcs.at(0));
						currentSheet->writeNum(10, 12 + bodyCountIndex * 11, orderedNumForceCalcs.at(orderedNumForceCalcs.size() / 4));
						currentSheet->writeNum(11, 12 + bodyCountIndex * 11, orderedNumForceCalcs.at(orderedNumForceCalcs.size() / 2));
						currentSheet->writeNum(12, 12 + bodyCountIndex * 11, orderedNumForceCalcs.at((orderedNumForceCalcs.size() * 3) / 4));
						currentSheet->writeNum(13, 12 + bodyCountIndex * 11, orderedNumForceCalcs.at(orderedNumForceCalcs.size() - 1));


						numForceCalcMedian = orderedNumForceCalcs.at(orderedNumForceCalcs.size() / 2);


						if (currentSettings.collision) {
							// NUM OF COLLISION CHECKS

							// raw data
							currentSheet->writeStr(15, 13 + bodyCountIndex * 11, "Num Collision Checks");
							std::vector<int> numCollisionChecks = simulation_->GetNumCollisionChecks();
							for (int i = 0; i < numCollisionChecks.size(); i++) {

								currentSheet->writeNum(16 + i, 13 + bodyCountIndex * 11, numCollisionChecks.at(i));
							}


							// averages
							currentSheet->writeStr(8, 13 + bodyCountIndex * 11, "Num Collision Checks");

							std::vector<int> orderedNumCollisionChecks = Sort(numCollisionChecks);
							currentSheet->writeNum(9, 13 + bodyCountIndex * 11, orderedNumCollisionChecks.at(0));
							currentSheet->writeNum(10, 13 + bodyCountIndex * 11, orderedNumCollisionChecks.at(orderedNumCollisionChecks.size() / 4));
							currentSheet->writeNum(11, 13 + bodyCountIndex * 11, orderedNumCollisionChecks.at(orderedNumCollisionChecks.size() / 2));
							currentSheet->writeNum(12, 13 + bodyCountIndex * 11, orderedNumCollisionChecks.at((orderedNumCollisionChecks.size() * 3) / 4));
							currentSheet->writeNum(13, 13 + bodyCountIndex * 11, orderedNumCollisionChecks.at(orderedNumCollisionChecks.size() - 1));


							numCollisionCheckMedian = orderedNumCollisionChecks.at(orderedNumCollisionChecks.size() / 2);

						}
#endif
					}
					else {

						std::cout << "writing results failed, current sheet doesn't exist" << std::endl;
						std::cout << "press any key to continue";
						_getch();
						std::cout << std::endl << std::endl;
					}



					if (summarySheet) {


						summarySheet->writeStr(4 + bodyCountIndex * 12, 5, "Num Bodies");
						summarySheet->writeNum(5 + bodyCountIndex * 12, 5, currentSettings.bodyCount);


						summarySheet->writeStr(4 + bodyCountIndex * 12, 8, "Threads");
						summarySheet->writeNum(5 + bodyCountIndex * 12, 8 + threadIndex * 2, currentSettings.threadCount);
						summarySheet->writeStr(6 + bodyCountIndex * 12, 8 + threadIndex * 2, "t");
						summarySheet->writeStr(6 + bodyCountIndex * 12, 9 + threadIndex * 2, "%");

						summarySheet->writeStr(7 + bodyCountIndex * 12, 7, "Force");
						summarySheet->writeStr(8 + bodyCountIndex * 12, 7, "Collision");
						summarySheet->writeStr(9 + bodyCountIndex * 12, 7, "Integration");
						summarySheet->writeStr(10 + bodyCountIndex * 12, 7, "Insert");
						summarySheet->writeStr(11 + bodyCountIndex * 12, 7, "Sort");
						summarySheet->writeStr(12 + bodyCountIndex * 12, 7, "Total");



						// write total time
						int totalTime = forceMedian + collisionMedian + integrationMedian + insertMedian + sortMedian;

						summarySheet->writeNum(12 + bodyCountIndex * 12, 8 + threadIndex * 2, totalTime);
						summarySheet->writeNum(12 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * totalTime / totalTime);

						// Force calc time
						summarySheet->writeNum(7 + bodyCountIndex * 12, 8 + threadIndex * 2, forceMedian);
						summarySheet->writeNum(7 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * forceMedian / totalTime);

						if (currentSettings.collision) {

							// Collision calc time
							summarySheet->writeNum(8 + bodyCountIndex * 12, 8 + threadIndex * 2, collisionMedian);
							summarySheet->writeNum(8 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * collisionMedian / totalTime);
						}

						// Integration calc time
						summarySheet->writeNum(9 + bodyCountIndex * 12, 8 + threadIndex * 2, integrationMedian);
						summarySheet->writeNum(9 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * integrationMedian / totalTime);

						// insert calc time
						summarySheet->writeNum(10 + bodyCountIndex * 12, 8 + threadIndex * 2, insertMedian);
						summarySheet->writeNum(10 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * insertMedian / totalTime);

						if (currentSettings.orderBodies) {

							// sort calc time
							summarySheet->writeNum(11 + bodyCountIndex * 12, 8 + threadIndex * 2, sortMedian);
							summarySheet->writeNum(11 + bodyCountIndex * 12, 9 + threadIndex * 2, 100 * sortMedian / totalTime);
						}


						// write num Force Calcs
						summarySheet->writeStr(7 + bodyCountIndex * 12, 5, "Num Force Calc");
						summarySheet->writeNum(8 + bodyCountIndex * 12, 5, numForceCalcMedian);

						if (currentSettings.collision) {

							// write num Collision Check
							summarySheet->writeStr(10 + bodyCountIndex * 12, 5, "Num Collision Check");
							summarySheet->writeNum(11 + bodyCountIndex * 12, 5, numCollisionCheckMedian);
						}
					}
					else {

						std::cout << "writing results failed, summary sheet doesn't exist" << std::endl;
						std::cout << "press any key to continue";
						_getch();
						std::cout << std::endl << std::endl;
					}


				}
				else {

					std::cout << "writing results failed, book doesn't exist" << std::endl;
					std::cout << "press any key to continue";
					_getch();
					std::cout << std::endl << std::endl;
				}


				if (!currentSettings.varyBodies) {

					break;
				}
			}

			if (!currentSettings.varyMultiThreading) {

				break;
			}

			threadIndex++;
		}

		std::cout << "saving results..." << std::endl;

		if (outputBook_) {

			std::string fileName = currentSettings.simName + ".xls";

			outputBook_->save(fileName.c_str());
			outputBook_->release();
			outputBook_ = nullptr;

			std::cout << "saving complete" << std::endl << std::endl;
		}
		else {

			std::cout << "save failed" << std::endl;
			std::cout << "press any key to continue";
			_getch();
			std::cout << std::endl << std::endl;
		}

	}


	std::cout << std::endl << "all testing completed!" << std::endl << std::endl;


	
}


void Benchmarker::CheckInput() {


}



void Benchmarker::StartSimulation(SimulationSettings* settings) {

	if (simulation_) {

		EndSimulation();
	}

	switch (settings->simMethod) {
	case Direct:
		simulation_ = new BruteForce();
		break;
	case Barnes_Hut:
		simulation_ = new BarnesHutCPU();
		break;
	default:
		simulation_ = new BruteForce();
		break;
	}

	simulation_->NewSettings(*settings);
	simulation_->Init();
	simulation_->Reset();
}


void Benchmarker::EndSimulation() {


	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}
}
