#include "Body.h"

#include "PhysicsUtil.h"
#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>
//#include "Renderer.h"


Body::Body() :
	force_(0.0f, 0.0f, 0.0f),
	mass_(1.0f)
	//sprite_(nullptr)
{
}


Body::~Body()
{

	//if (sprite_) {

	//	delete sprite_;
	//	sprite_ = nullptr;
	//}
}


void Body::Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass) {


	SetColour(sf::Vector3f(0.0f, 0.0f, 0.0f));
	
	SetMass(newMass);
	ResetForce();

	currentState_.position_ = newPos;
	currentState_.velocity_ = newVel;

	prevState_.position_ = newPos;
	prevState_.velocity_ = newVel;

	//sprite_ = new sf::CircleShape(20.0f);// sqrt(mass_ / PhysicsUtil::pi));
	//sprite_->setOrigin(sf::Vector2f(sprite_->getRadius(), sprite_->getRadius()));
	//sprite_->setScale(0.1f, 0.1f);

}


void Body::SetColour(sf::Vector3f rgb) {

	colour_ = rgb;
}



void Body::AddForce(sf::Vector3f newForce) {

	force_ += newForce;
}


void Body::AddForce(Body* body) {

	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(CurrentState().position_, body->CurrentState().position_);
	float distance = PhysicsUtil::Normalise(distanceVector);
	distance += PhysicsUtil::dampeningFactor;

	sf::Vector3f force = (PhysicsUtil::G * Mass() * body->Mass() / (distance * distance * distance)) * distanceVector;

	AddForce(force);
}

void Body::AddForce(sf::Vector3f bodyPos, float bodyMass) {

	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(CurrentState().position_, bodyPos);
	float distance = PhysicsUtil::Normalise(distanceVector);
	distance += PhysicsUtil::dampeningFactor;

	sf::Vector3f force = (PhysicsUtil::G * Mass() * bodyMass / (distance * distance * distance)) * distanceVector;

	AddForce(force);
}


void Body::ResetForce() {

	force_.x = 0.0f;
	force_.y = 0.0f;
	force_.z = 0.0f;
}


void Body::Integrate_SemiImplicitEuler(float dt) {

	prevState_ = currentState_;

	currentState_.velocity_ += (force_ / mass_) * dt;
	currentState_.position_ += currentState_.velocity_ * dt;
}


State Body::InterpolateState(float alpha) {

	return (currentState_ * alpha) + (prevState_ * (1.0f - alpha));
}


void Body::Draw(float alpha) {

	State bodyState = InterpolateState(alpha);

	glPushMatrix();
	
		glColor3f(colour_.x, colour_.y, colour_.z);
		glTranslatef(bodyState.position_.x, bodyState.position_.y, bodyState.position_.z);
		glutSolidSphere(20, 20, 20);

	glPopMatrix();

	//sprite_->setPosition(spriteState.position_);

	//renderer->Draw(*sprite_);
}



void Body::SetMass(float newMass) {

	mass_ = newMass;

	modelRadius_ = 20.0f;// std::cbrtf(3.0f * mass_ / (4.0f * PhysicsUtil::pi));

}
