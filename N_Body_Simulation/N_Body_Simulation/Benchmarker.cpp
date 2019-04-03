#include "Benchmarker.h"


// std library includes
#include <iostream>

// My class includes
#include "Simulation.h"
#include "Input.h"
#include "BarnesHutCPU.h"
#include "BruteForce.h"


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

	input_ = newInput;

	outputBook_ = xlCreateBook();

	CreateSimulationSettings();
}


void Benchmarker::CreateSimulationSettings() {

	
	SimulationSettings test1;
	test1.collision = true;
	test1.dt = 1.0f / 15.0f;
	test1.integrationMethod = Semi_Implicit_Euler;
	test1.multiThreading = true;
	test1.threadCount = 12;
	test1.timingSteps = true;
	test1.simMethod = Barnes_Hut;
	test1.simMode = Random_Bodies;
	test1.simName = "I'm testing poop";
	test1.varyBodies = false;
	test1.bodyCount = 100;
	test1.varyMultiThreading = true;
	test1.maxSteps = 100;
	test1.partitionSize = 10000.0f;
	test1.theta = 1.0f;

	benchmarkSettingsList_.push_back(test1);
}



void Benchmarker::MainLoop() {


	// run a simulation for each setting
	for (auto currentSettings : benchmarkSettingsList_) {

		// loop for each thread count in range (1, 2, 4, 6, 8, 10, 12...)
		for (; currentSettings.threadCount <= std::thread::hardware_concurrency(); (currentSettings.threadCount == 1) ? currentSettings.threadCount++ : currentSettings.threadCount += 2) {

			// loop for each body count in body count list
			for (int bodyCountIndex = 0; bodyCountIndex < 20; bodyCountIndex++) {

				// update body count for current simulation
				if (currentSettings.varyBodies) {
					currentSettings.bodyCount = currentSettings.bodyCountList[bodyCountIndex];
				}

				
				StartSimulation(&currentSettings);


				// do a timestep until max has been hit
				for (int i = 0; i < currentSettings.maxSteps; i++) {

					simulation_->TimeStep(currentSettings.dt);
				}


				// write results into excel spreadsheet

				if (outputBook_) {

					std::string testName = currentSettings.simName
						+ " thread " + std::to_string(currentSettings.threadCount)
						+ " bodyCount " + std::to_string(currentSettings.bodyCount);

					libxl::Sheet* sheet = outputBook_->addSheet(testName.c_str());

					if (sheet) {
						// write settings list
						sheet->writeStr(6, 2, "Settings");

						sheet->writeStr(8, 2, "Method");
						switch (currentSettings.simMethod) {
						case Direct:
							sheet->writeStr(8, 3, "Direct");
							break;
						case Barnes_Hut:
							sheet->writeStr(8, 3, "Barnes-Hut");
							break;
						default:
							sheet->writeStr(8, 3, "Error");
							break;
						}


						sheet->writeStr(9, 2, "Mode");
						switch (currentSettings.simMode) {
						case Random_Bodies:
							sheet->writeStr(9, 3, "Random");
							break;
						case Two_Body_Orbit:
							sheet->writeStr(9, 3, "Two_Body_Orbit");
							break;
						case Even_Distribution:
							sheet->writeStr(9, 3, "Even Distribution");
							break;
						case Clustered_Distribution:
							sheet->writeStr(9, 3, "Clustered Distribution");
							break;
						case Asteroids:
							sheet->writeStr(9, 3, "Asteroids");
							break;
						default:
							sheet->writeStr(9, 3, "Error");
							break;
						}

						sheet->writeStr(10, 2, "Integration");
						switch (currentSettings.integrationMethod) {
						case Semi_Implicit_Euler:
							sheet->writeStr(10, 3, "Semi-Implicit Euler");
							break;
						case Explicit_Euler:
							sheet->writeStr(10, 3, "Explicit Euler");
							break;
						case Verlet:
							sheet->writeStr(10, 3, "Verlet");
							break;
						case Runge_Kutta:
							sheet->writeStr(10, 3, "4th Order Runge-Kutta");
							break;
						default:
							sheet->writeStr(10, 3, "Error");
							break;
						}

						sheet->writeStr(12, 2, "Total Steps");
						sheet->writeNum(12, 3, currentSettings.maxSteps);

						sheet->writeStr(13, 2, "Num Bodies");
						sheet->writeNum(13, 3, currentSettings.bodyCount);

						sheet->writeStr(14, 2, "Threads");
						sheet->writeNum(14, 3, currentSettings.threadCount);

						sheet->writeStr(15, 2, "Collision");
						sheet->writeBool(15, 3, currentSettings.collision);

						sheet->writeStr(16, 2, "Time Step");
						sheet->writeNum(16, 3, currentSettings.dt);

						sheet->writeStr(17, 2, "Theta");
						sheet->writeNum(17, 3, currentSettings.theta);


						// Label table

						// time step count
						sheet->writeStr(8, 5, "Time Step");
						for (int i = 0; i < currentSettings.maxSteps; i++) {

							sheet->writeNum(9 + i, 5, i);
						}

#if BENCHMARKING

						// Timing labels
						sheet->writeStr(6, 6, "Timing");


						sheet->writeStr(8, 6, "Force Time");

						std::vector<int> forceCalcTimes = simulation_->GetForceCalcTimes();
						for (int i = 0; i < forceCalcTimes.size(); i++) {

							sheet->writeNum(9 + i, 6, forceCalcTimes.at(i));
						}



						sheet->writeStr(8, 7, "Collision Check Time");

						std::vector<int> collisionCheckTimes = simulation_->GetCollisionCheckTimes();
						for (int i = 0; i < collisionCheckTimes.size(); i++) {

							sheet->writeNum(9 + i, 7, collisionCheckTimes.at(i));
						}



						sheet->writeStr(8, 8, "Integration Time");

						std::vector<int> integrationTimes = simulation_->GetIntegrationTimes();
						for (int i = 0; i < integrationTimes.size(); i++) {

							sheet->writeNum(9 + i, 8, integrationTimes.at(i));
						}




						if (currentSettings.simMethod == Barnes_Hut) {


							sheet->writeStr(8, 9, "Insert Time");

							std::vector<int> insertTimes = ((BarnesHutCPU*)simulation_)->GetInsertTimes();
							for (int i = 0; i < insertTimes.size(); i++) {

								sheet->writeNum(9 + i, 9, insertTimes.at(i));
							}



							sheet->writeStr(8, 10, "Sort Time");

							std::vector<int> sortTimes = ((BarnesHutCPU*)simulation_)->GetSortTimes();
							for (int i = 0; i < sortTimes.size(); i++) {

								sheet->writeNum(9 + i, 10, sortTimes.at(i));
							}

						}



						sheet->writeStr(6, 12, "Num Operations");


						sheet->writeStr(8, 12, "Num Force Calculations");
						std::vector<int> numForceCalcs = simulation_->GetNumForceCalcs();
						for (int i = 0; i < numForceCalcs.size(); i++) {

							sheet->writeNum(9 + i, 12, numForceCalcs.at(i));
						}



						sheet->writeStr(8, 13, "Num Collision Checks");
						std::vector<int> numCollisionChecks = simulation_->GetNumCollisionChecks();
						for (int i = 0; i < numCollisionChecks.size(); i++) {

							sheet->writeNum(9 + i, 13, numCollisionChecks.at(i));
						}



#endif
					}
				}


				if (!currentSettings.varyBodies) {

					break;
				}
			}

			if (!currentSettings.varyMultiThreading) {

				break;
			}

		}
	}

	if (outputBook_) {

		outputBook_->save("Results.xls");
		outputBook_->release();
		outputBook_ = nullptr;
	}
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
