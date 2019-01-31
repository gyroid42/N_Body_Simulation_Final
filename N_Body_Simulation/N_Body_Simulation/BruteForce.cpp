#include "BruteForce.h"



BruteForce::BruteForce()
{
}


BruteForce::~BruteForce()
{
}



void BruteForce::TimeStep(float dt) {


	// Evaluate each body against each other body
	for (auto i = bodies_.begin(); i != bodies_.end(); i++) {

		(*i)->ResetForce();
		for (auto j = bodies_.begin(); j != bodies_.end(); j++) {

			if (i != j) {

				PhysicsUtil::AddForcesBetween(*i, *j);
				//(*i)->AddForce(*j);
			}
		}

	}

	for (auto i = bodies_.begin(); i != bodies_.end(); i++) {

		(*i)->Integrate_SemiImplicitEuler(dt);
	}

}
