#include "Body.h"

#include "PhysicsUtil.h"
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

	
	mass_ = newMass;
	ResetForce();

	currentState_.position_ = newPos;
	currentState_.velocity_ = newVel;

	prevState_.position_ = newPos;
	prevState_.velocity_ = newVel;

	//sprite_ = new sf::CircleShape(20.0f);// sqrt(mass_ / PhysicsUtil::pi));
	//sprite_->setOrigin(sf::Vector2f(sprite_->getRadius(), sprite_->getRadius()));
	//sprite_->setScale(0.1f, 0.1f);

}


//void Body::SetColour(sf::Color newColor) {

//	sprite_->setFillColor(newColor);
//}



void Body::AddForce(sf::Vector3f newForce) {

	force_ += newForce;
}


void Body::ResetForce() {

	force_.x = 0.0f;
	force_.y = 0.0f;
}


void Body::Integrate_SemiImplicitEuler(float dt) {

	prevState_ = currentState_;

	currentState_.velocity_ += (force_ / mass_) * dt;
	currentState_.position_ += currentState_.velocity_ * dt;
}


State Body::InterpolateState(float alpha) {

	return (currentState_ * alpha) + (prevState_ * (1.0f - alpha));
}


void Body::Draw(Renderer* renderer, float alpha) {

	State spriteState = InterpolateState(alpha);
	//sprite_->setPosition(spriteState.position_);

	//renderer->Draw(*sprite_);
}

