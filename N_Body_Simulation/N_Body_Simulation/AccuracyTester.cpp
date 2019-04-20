#include "AccuracyTester.h"


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


AccuracyTester::AccuracyTester() :
	input_(nullptr),
	simulation_(nullptr),
	outputBook_(nullptr)
{
}


AccuracyTester::~AccuracyTester()
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


void AccuracyTester::Init(Input* newInput) {

	std::cout << "Initialising accuracy tester.." << std::endl;

	input_ = newInput;

	outputBook_ = nullptr;

	std::cout << "Creating test scenarios..." << std::endl;

	CreateSimulationSettings();

	std::cout << "Finished creating test scenarios" << std::endl;

	std::cout << "FInished initialising" << std::endl;
}



void AccuracyTester::CreateSimulationSettings() {


	SimulationSettings meh;
	meh.simName = "meh";
	meh.satelliteMass = 100.0f;
	meh.planetMass = 1.45E+22f;
	meh.orbitStartPos = sf::Vector3f(1.0E+7f, 0.0f, 0.0f);

	simSettings_.push_back(meh);

}


void AccuracyTester::MainLoop() {

	std::cout << std::endl << std::endl << "Starting accuracy testing" << std::endl;


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

				currentSheet->writeStr(17, 2, "Theta");
				currentSheet->writeNum(17, 3, currentSettings.theta);

				currentSheet->writeStr(18, 2, "Time steps per orbit");
				currentSheet->writeNum(18, 3, currentSettings.stepsPerPeriod);

				currentSheet->writeStr(19, 2, "period length");
				currentSheet->writeNum(19, 3, currentSettings.periodLength);



				currentSheet->writeStr(4, 5, "Satellite Mass");
				currentSheet->writeNum(5, 5, currentSettings.satelliteMass);

				currentSheet->writeStr(4, 7, "Planet Mass");
				currentSheet->writeNum(5, 7, currentSettings.planetMass);

				currentSheet->writeStr(4, 9, "Actual Orbit Distance");
				currentSheet->writeNum(5, 9, PhysicsUtil::VectorLength(currentSettings.orbitStartPos));

				currentSheet->writeStr(7, 5, "Time Step");
				currentSheet->writeNum(8, 5, currentSettings.dt);

				currentSheet->writeStr(7, 7, "Actual Solution Energy");
				currentSheet->writeFormula(8, 7, "=-J9*H6*F6/(2*J6)");

				currentSheet->writeStr(7, 9, "Gravitational Constant");
				currentSheet->writeNum(8, 9, PhysicsUtil::G);

				
				currentSheet->writeStr(10, 5, "Period");

				currentSheet->writeStr(10, 6, "Distance From Solution");
				currentSheet->writeStr(10, 7, "Orbit Distance");
				currentSheet->writeStr(10, 8, "Distance From Actual Orbit");
				currentSheet->writeFormula(11, 8, "=H12-J6");
				currentSheet->writeStr(10, 9, "Energy");
				currentSheet->writeStr(10, 10, "Delta Energy");
				currentSheet->writeFormula(11, 8, "=J12-H9");
			}
		}

		std::string fileName = currentSettings.simName + ".xls";

		outputBook_->save(fileName.c_str());


		// for each period
		for (int period = 0; period < currentSettings.numPeriods; period++) {

			for (int i = 0; i < currentSettings.stepsPerPeriod; i++) {

				simulation_->TimeStep(currentSettings.dt);
			}


			// add data to current sheet
			if (currentSheet) {

				currentSheet->writeNum(11 + period, 5, period);

				Body* satellite = simulation_->GetBody("satellite");

				if (!satellite) {

					std::cout << "couldn't find satellite body" << std::endl;
					std::cout << "press any key to continue";
					_getch();
					std::cout << std::endl << std::endl;
				}

				if (satellite) {

					sf::Vector3f distanceVector = satellite->Position() - currentSettings.orbitStartPos;

					currentSheet->writeNum(11 + period, 6, PhysicsUtil::VectorLength(distanceVector));


					currentSheet->writeNum(11 + period, 7, PhysicsUtil::VectorLength(satellite->Position));

					// calculate energy
					float kineticEnergy = 0.5f * satellite->Mass() * PhysicsUtil::VectorLengthSqr(satellite->Velocity());
					float potentialEnergy = -PhysicsUtil::G * currentSettings.planetMass * satellite->Mass() / PhysicsUtil::VectorLength(satellite->Position());

					float energy = kineticEnergy + potentialEnergy;

					currentSheet->writeNum(11 + period, 9, energy);
				}

			}
			
			// get distance between body and actual solution

			// get orbit distance

			

		}

		// for number of time steps per period
		// simulate a step
		// end for

		// add period and data to current sheet

		// end for



	}
}



void AccuracyTester::CheckInput() {

}




void AccuracyTester::StartSimulation(SimulationSettings* settings) {

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



void AccuracyTester::EndSimulation() {

	if (simulation_) {

		simulation_->CleanUp();
		delete simulation_;
		simulation_ = nullptr;
	}
}

