#include "BruteForce.h"



BruteForce::BruteForce()
{
}


BruteForce::~BruteForce()
{
}



void BruteForce::TimeStep(float dt) {


	// Evaluate each body against each other body
	/*for (auto i = bodies_.begin(); i != bodies_.end(); i++) {

		(*i)->ResetForce();
		for (auto j = bodies_.begin(); j != bodies_.end(); j++) {

			if (i != j) {

				PhysicsUtil::AddForcesBetween(*i, *j);
				//(*i)->AddForce(*j);
			}
		}

	}*/


	for (auto body1 : bodies_) {

		body1->ResetForce();
		for (auto body2 : bodies_) {

			if (body1 != body2) {

				PhysicsUtil::AddForcesBetween(body1, body2);
			}
		}
	}


	for (auto body : bodies_) {

		body->Integrate_SemiImplicitEuler(dt);
	}

}


void BruteForce::CalculateForceOnBody(Body* body1) {


	body1->ResetForce();
	for (auto body2 : bodies_) {

		if (body1 != body2) {

			PhysicsUtil::AddForcesBetween(body1, body2);
		}
	}

}
