#include "BarnesHutAccuracyTester.h"


// std library includes
#include <iostream>
#include <conio.h>

// my class includes
#include "Simulation.h"
#include "Input.h"
#include "BarnesHutCPU.h"
#include "BruteForce.h"
#include "SortVector.h"
#include "PhysicsUtil.h"


BarnesHutAccuracyTester::BarnesHutAccuracyTester() :
	input_(nullptr),
	simulation_(nullptr),
	outputBook_(nullptr)
{
}


BarnesHutAccuracyTester::~BarnesHutAccuracyTester()
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


void BarnesHutAccuracyTester::Init(Input* newInput) {

	std::cout << "Initialising accuracy tester.." << std::endl;

	input_ = newInput;

	outputBook_ = nullptr;

	std::cout << "Creating test scenarios..." << std::endl;

	CreateSimulationSettings();

	std::cout << "Finished creating test scenarios" << std::endl;

	std::cout << "FInished initialising" << std::endl;
}


void BarnesHutAccuracyTester::CreateSimulationSettings() {


	SimulationSettings thetaTest;

	thetaTest.simName = "BarnesHutAccuracy";

	thetaTest.collision = false;

	thetaTest.integrationMethod = Semi_Implicit_Euler;

	thetaTest.simMethod = Barnes_Hut;
	thetaTest.theta = 1.0f;
	thetaTest.orderBodies = true;

	thetaTest.simMode = Clustered_Distribution;
	thetaTest.varyBodies = false;
	thetaTest.bodyCount = 1000;

	thetaTest.dt = 1.0f / 5.0f;


	thetaTest.threadCount = 12;
	thetaTest.multiThreading = true;
	thetaTest.varyMultiThreading = false;

	thetaTest.timingSteps = true;


	thetaTest.maxSteps = 100;
	thetaTest.partitionSize = 10000.0f;

	simSettings_.push_back(thetaTest);
}


void BarnesHutAccuracyTester::MainLoop() {

	std::cout << std::endl << std::endl << "Starting Barnes-Hut accuracy testing" << std::endl;


	for (auto currentSettings : simSettings_) {

		outputBook_ = xlCreateBook();

		std::cout << std::endl << "running " << currentSettings.simName << " test..." << std::endl;

		libxl::Sheet* currentSheet = nullptr;

		if (outputBook_) {

			currentSheet = outputBook_->addSheet(currentSettings.simName.c_str());

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

				currentSheet->writeStr(17, 2, "Body Count");
				currentSheet->writeNum(17, 3, currentSettings.bodyCount);

			}
		}


		std::cout << "Testing Direct Method..." << std::endl;

		// simulate direct method
		std::vector<std::vector<sf::Vector3f>*> directBodyPositions;

		currentSettings.simMethod = Direct;

		StartSimulation(&currentSettings);


		for (int i = 0; i < 5; i++) {

			for (int timeStep = 0; timeStep < 10; timeStep++) {

				simulation_->TimeStep(currentSettings.dt);
			}

			std::cout << "10 steps complete" << std::endl;

			std::cout << "storing positions..." << std::endl;
			directBodyPositions.push_back(new std::vector<sf::Vector3f>);

			// collect data
			for (int b = 0; b < currentSettings.bodyCount; b++) {

				std::string bodyName = std::to_string(b);
				directBodyPositions.at(i)->push_back(simulation_->GetBody(bodyName)->Position());
			}
			std::cout << "positions stored" << std::endl;
		}


		std::cout << "Finished direct method" << std::endl << std::endl;


		currentSettings.simMethod = Barnes_Hut;
		// simulate each barnes hut

		int testIndex = 0;

		for (float newTheta = 0.6f; newTheta <= 2.0f; newTheta += 0.2f) {

			currentSettings.theta = newTheta;

			std::cout << "Testing with theta = " << newTheta << "..." << std::endl;

			// write table headings
			currentSheet->writeStr(5, 6 + 8 * testIndex, "theta");
			currentSheet->writeNum(6, 6 + 8 * testIndex, currentSettings.theta);


			currentSheet->writeStr(8, 7 + 8 * testIndex, "distance from solution");

			currentSheet->writeStr(9, 6 + 8 * testIndex, "time step");
			currentSheet->writeStr(9, 7 + 8 * testIndex, "Min");
			currentSheet->writeStr(9, 8 + 8 * testIndex, "Q1");
			currentSheet->writeStr(9, 9 + 8 * testIndex, "Median");
			currentSheet->writeStr(9, 10 + 8 * testIndex, "Q3");
			currentSheet->writeStr(9, 11 + 8 * testIndex, "Max");



			StartSimulation(&currentSettings);

			for (int i = 0; i < 5; i++) {

				for (int timeStep = 0; timeStep < 10; timeStep++) {

					simulation_->TimeStep(currentSettings.dt);
				}


				std::vector<float> distanceUnsorted;
				std::vector<float> distanceSorted;

				for (int b = 0; b < currentSettings.bodyCount; b++) {

					std::string bodyName = std::to_string(b);

					sf::Vector3f bodyPos = simulation_->GetBody(bodyName)->Position();
					sf::Vector3f distanceVector = bodyPos - directBodyPositions.at(i)->at(b);
					distanceUnsorted.push_back(PhysicsUtil::VectorLength(distanceVector));
				}

				distanceSorted = Sort(distanceUnsorted);

				float min = distanceSorted.at(0);
				float Q1 = distanceSorted.at(distanceSorted.size() / 4);
				float median = distanceSorted.at(distanceSorted.size() / 2);
				float Q3 = distanceSorted.at((distanceSorted.size() * 3) / 4);
				float max = distanceSorted.at(distanceSorted.size() - 1);

				// get each body position
				// store difference into vector
				// calculate min, Q1, Median, Q3 and Max
				// write to sheet

				currentSheet->writeNum(10 + i, 6 + 8 * testIndex, i * 10);
				currentSheet->writeNum(10 + i, 7 + 8 * testIndex, min);
				currentSheet->writeNum(10 + i, 8 + 8 * testIndex, Q1);
				currentSheet->writeNum(10 + i, 9 + 8 * testIndex, median);
				currentSheet->writeNum(10 + i, 10 + 8 * testIndex, Q3);
				currentSheet->writeNum(10 + i, 11 + 8 * testIndex, max);


			}


			testIndex++;

			std::cout << "Finished theta = " << newTheta << std::endl << std::endl;
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

	std::cout << "Finished all tests" << std::endl;

}

void BarnesHutAccuracyTester::CheckInput() {

}




void BarnesHutAccuracyTester::StartSimulation(SimulationSettings* settings) {

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



void BarnesHutAccuracyTester::EndSimulation() {

	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}
}


