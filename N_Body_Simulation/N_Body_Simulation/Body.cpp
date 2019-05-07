
// include class header
#include "Body.h"

// glut includes
#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>


// my class includes
#include "PhysicsUtil.h"

Body::Body() :
	force_(0.0f, 0.0f, 0.0f),
	mass_(1.0f)
{
}


Body::~Body()
{
}


void Body::Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass, INTEGRATION_METHOD integrationMethod) {

	// Set default body colour
	SetColour(sf::Vector3f(0.0f, 0.0f, 0.0f));
	
	// Set body mass
	SetMass(newMass);

	// Reset the force acting on the body
	ResetForce();

	// Update previous and current states
	states_[0].position_ = newPos;
	states_[0].velocity_ = newVel;

	states_[1].position_ = newPos;
	states_[1].velocity_ = newVel;

	states_[2].position_ = newPos;
	states_[2].velocity_ = newVel;

	states_[3].position_ = newPos;
	states_[3].velocity_ = newVel;

	initialCounter_ = 0;

	nextBody_ = nullptr;
	collisionTreeInserted_ = false;



	switch (integrationMethod) {
	case Semi_Implicit_Euler:
		Integrate = &Body::Integrate_SemiImplicitEuler;
		break;
	case Explicit_Euler:
		Integrate = &Body::Integrate_ExplicitEuler;
		break;
	case Verlet:
		Integrate = &Body::Integrate_VerletStart;
		break;
	case Velocity_Verlet:
		Integrate = &Body::Integrate_VelVerletStart;
		break;
	default:
		Integrate = &Body::Integrate_SemiImplicitEuler;
		break;
	}

}



void Body::AddForce(sf::Vector3f newForce) {

	force_ += newForce;
}

void Body::AddForce(Body* body) {

	// applies force to body due to another body

	// get the distance between the 2 bodies
	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(CurrentState().position_, body->CurrentState().position_);
	float distance = PhysicsUtil::Normalise(distanceVector);
	distance += PhysicsUtil::dampeningFactor;

	// use F = G*m1*m2 / (d^3) to calculate the force
	// use the normalised distance vector to get the direction
	sf::Vector3f force = (PhysicsUtil::G * Mass() * body->Mass() / (distance * distance)) * distanceVector;

	// Apply force calculated to body
	AddForce(force);
}

void Body::AddForce(sf::Vector3f bodyPos, float bodyMass) {

	// applies force to body given another bodies position and mass

	// get the distance between the 2 bodies
	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(CurrentState().position_, bodyPos);
	float distance = PhysicsUtil::Normalise(distanceVector);
	//float distance = PhysicsUtil::VectorLength(distanceVector);
	distance += PhysicsUtil::dampeningFactor;

	// use F = G*m1*m2 / (d^3) to calculate the force
	// use the normalised distance vector to get the direction
	sf::Vector3f force = (PhysicsUtil::G * Mass() * bodyMass / (distance * distance)) * distanceVector;

	// Apply force calculated to body
	AddForce(force);
}


void Body::ResetForce() {

	force_.x = 0.0f;
	force_.y = 0.0f;
	force_.z = 0.0f;
}


void Body::Integrate_ExplicitEuler(float dt) {

	//states_[1] = states_[0];

	acceleration_ = force_ / mass_;

	states_[0].position_ += states_[0].velocity_ * dt;
	states_[0].velocity_ += acceleration_ * dt;
	
}

void Body::Integrate_SemiImplicitEuler(float dt) {

	// set previous state before integrating
	//prevStates_[0] = currentState_;

	acceleration_ = force_ / mass_;

	// calculate velocity then position linearly
	states_[0].velocity_ += acceleration_ * dt;
	states_[0].position_ += states_[0].velocity_ * dt;
}

void Body::Integrate_Verlet(float dt) {


	// set previous states from last frame
	//prevStates_[1] = prevStates_[0];
	//prevStates_[0] = currentState_;

	// get next position
	states_[0].position_ = 2.0f * states_[1].position_ - states_[2].position_ + (force_ / mass_) * dt * dt;

	// get next velocity
	states_[0].velocity_ = (states_[0].position_ - states_[2].position_) / (2.0f * dt);
	
}

void Body::Integrate_VerletStart(float dt) {

	Integrate_SemiImplicitEuler(dt);

	initialCounter_++;

	if (initialCounter_ >= 2) {

		Integrate = &Body::Integrate_Verlet;
	}
}



void Body::Integrate_VelocityVerlet(float dt) {

	//prevStates_[0] = currentState_;

	//sf::Vector3f halfVel = currentState_.velocity_ + 0.5f * dt * acceleration_;
	//currentState_.position_ += dt * halfVel;

	acceleration_ = force_ / mass_;
	//currentState_.velocity_ = halfVel + 0.5f * dt * acceleration_;
}


void Body::Integrate_VelVerletStart(float dt) {

	Integrate_SemiImplicitEuler(dt);

	initialCounter_++;

	if (initialCounter_ >= 2) {

		Integrate = &Body::Integrate_VelocityVerlet;
	}
}



State Body::InterpolateState(float alpha) {

	// Larps between the previous state and current state given alpha (0-1)
	return (states_[1] * alpha) + (states_[2] * (1.0f - alpha));
}


void Body::ShiftStates() {

	states_[2] = states_[1];
	states_[1] = states_[0];
}


void Body::Draw(float alpha) {

	// get a new state between the current and previous
	State bodyState = InterpolateState(alpha);

	// translate to new position and draw body
	glPushMatrix();
	
		glColor3f(colour_.x, colour_.y, colour_.z);
		glTranslatef(bodyState.position_.x, bodyState.position_.y, bodyState.position_.z);
		glutSolidSphere(20, 20, 20);

	glPopMatrix();
}



void Body::SetMass(float newMass) {

	// set new mass
	mass_ = newMass;

	// calculate new radius of body
	modelRadius_ = 20.0f;// std::cbrtf(3.0f * mass_ / (4.0f * PhysicsUtil::pi));

}



void Body::MergeBody(Body* b) {

	// add forces
	// combine current state
	// combine mass

	//force_ += b->Force();
	State bState = b->CurrentState();
	float bMass = b->Mass();
	float combinedMass = mass_ + bMass;

	//sf::Vector3f distance = bState.position_ - currentState_.position_;

	//currentState_.position_ += distance * bMass / combinedMass;


	//sf::Vector3f deltaVel = bState.velocity_ - currentState_.velocity_;

	//currentState_.velocity_ -= deltaVel;
	//currentState_.velocity_ += (mass_*deltaVel + bMass * deltaVel) / combinedMass;

	//currentState_.position_ = (currentState_.position_ * mass_ + bState.position_ * bMass) / combinedMass;
	//currentState_.velocity_ = (currentState_.velocity_ * mass_ + bState.velocity_ * bMass) / combinedMass;

	mass_ = combinedMass;

}
