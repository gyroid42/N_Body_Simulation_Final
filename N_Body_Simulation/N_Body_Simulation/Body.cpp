
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


void Body::Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass) {

	// Set default body colour
	SetColour(sf::Vector3f(0.0f, 0.0f, 0.0f));
	
	// Set body mass
	SetMass(newMass);

	// Reset the force acting on the body
	ResetForce();

	// Update previous and current states
	currentState_.position_ = newPos;
	currentState_.velocity_ = newVel;

	prevStates_[0].position_ = newPos;
	prevStates_[0].velocity_ = newVel;

	prevStates_[1].position_ = newPos;
	prevStates_[1].velocity_ = newVel;

	initialCounter_ = 0;

#if INTEGRATION_METHOD == 0

	Integrate = &Body::Integrate_SemiImplicitEuler;

#endif

#if INTEGRATION_METHOD == 1

	Integrate = &Body::Integrate_VerletStart;

#endif
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


void Body::Integrate_SemiImplicitEuler(float dt) {

	// set previous state before integrating
	prevStates_[0] = currentState_;

	// calculate velocity then position linearly
	currentState_.velocity_ += (force_ / mass_) * dt;
	currentState_.position_ += currentState_.velocity_ * dt;
}

void Body::Integrate_Verlet(float dt) {


	// set previous states from last frame
	prevStates_[1] = prevStates_[0];
	prevStates_[0] = currentState_;

	// get next position
	currentState_.position_ = 2.0f * prevStates_[0].position_ - prevStates_[1].position_ + (force_ / mass_) * dt * dt;

	// get next velocity
	currentState_.velocity_ = (currentState_.position_ - prevStates_[1].position_) / (2.0f * dt);
	
}

void Body::Integrate_VerletStart(float dt) {

	Integrate_SemiImplicitEuler(dt);

	initialCounter_++;

	if (initialCounter_ >= 2) {

		Integrate = &Body::Integrate_Verlet;
	}
}


State Body::InterpolateState(float alpha) {

	// Larps between the previous state and current state given alpha (0-1)
	return (currentState_ * alpha) + (prevStates_[0] * (1.0f - alpha));
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
