
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

	prevState_.position_ = newPos;
	prevState_.velocity_ = newVel;

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
	sf::Vector3f force = (PhysicsUtil::G * Mass() * body->Mass() / (distance * distance * distance)) * distanceVector;

	// Apply force calculated to body
	AddForce(force);
}

void Body::AddForce(sf::Vector3f bodyPos, float bodyMass) {

	// applies force to body given another bodies position and mass

	// get the distance between the 2 bodies
	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(CurrentState().position_, bodyPos);
	float distance = PhysicsUtil::Normalise(distanceVector);
	distance += PhysicsUtil::dampeningFactor;

	// use F = G*m1*m2 / (d^3) to calculate the force
	// use the normalised distance vector to get the direction
	sf::Vector3f force = (PhysicsUtil::G * Mass() * bodyMass / (distance * distance * distance)) * distanceVector;

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
	prevState_ = currentState_;

	// calculate velocity then position linearly
	currentState_.velocity_ += (force_ / mass_) * dt;
	currentState_.position_ += currentState_.velocity_ * dt;
}


State Body::InterpolateState(float alpha) {

	// Larps between the previous state and current state given alpha (0-1)
	return (currentState_ * alpha) + (prevState_ * (1.0f - alpha));
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
