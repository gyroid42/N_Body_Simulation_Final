#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

#include <string>

// my class includes
#include "SettingsEnums.h"

// forward declarations
class Renderer;


// State struct stores a bodies position and velocity
struct State {

	sf::Vector3f position_;
	sf::Vector3f velocity_;


	State() { 
		position_.x = 0.0f;
		position_.y = 0.0f;
		position_.z = 0.0f;

		velocity_.x = 0.0f;
		velocity_.y = 0.0f;
		velocity_.z = 0.0f;
	}

	State(sf::Vector3f newPos, sf::Vector3f newVel) {

		position_ = newPos;
		velocity_ = newVel;
	}

	// Multiplies position and velocity by some scaler
	inline State operator*(float a) {

		return { position_ * a, velocity_ * a };
	}

	// Adds two states together
	inline State operator+(const State& a) {

		return { position_ + a.position_, velocity_ + a.velocity_ };
	}

};


class Body
{
public:
	Body();
	~Body();

	void Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass, INTEGRATION_METHOD integrationMethod);

	// Methods to add a force to the body
	void AddForce(sf::Vector3f newForce);
	void AddForce(Body* body);
	void AddForce(sf::Vector3f bodyPos, float bodyMass);

	// Resets the bodies force to 0
	void ResetForce();

	// Integration Methods
	void (Body::*Integrate)(float);

	void Integrate_SemiImplicitEuler(float dt);
	void Integrate_ExplicitEuler(float dt);

	void Integrate_Verlet(float dt);
	void Integrate_VerletStart(float dt);

	void Integrate_VelVerletStart(float dt);
	void Integrate_VelocityVerlet(float dt);


	// Interpolates between the previous state and current state
	State InterpolateState(float alpha);
	//void Integrate_Verlet();


	// Draws the body to the render target
	void Draw(float alpha);


	void MergeBody(Body* b);

	
	// Getters
	inline float Mass() { return mass_; }
	inline sf::Vector3f Position() { return currentState_.position_; }
	inline float ModelRadius() { return modelRadius_; }
	inline State CurrentState() { return currentState_; }
	inline Body* NextBody() { return nextBody_; }
	inline bool InsertedCollision() { return collisionTreeInserted_; }
	inline bool DestroyFlag() { return destroy_; }
	inline sf::Vector3f Force() { return force_; }
	inline std::string Name() { return name_; }
	inline sf::Vector3f Velocity() { return currentState_.velocity_; }


	// Setters
	inline void SetColour(sf::Vector3f rgb) { colour_ = rgb; }
	void SetMass(float newMass);
	inline void Destroy() { destroy_ = true; }
	inline void SetNextBody(Body* next) { nextBody_ = next; }
	inline void SetInsertedCollision(bool newValue) { collisionTreeInserted_ = newValue; }
	inline void SetName(std::string newName) { name_ = newName; }

private:

	std::string name_;

	int initialCounter_;

	// State of body in the previous physics step
	State prevStates_[2];

	// State of body in the current physics step
	State currentState_;


	// Force acting on the body this physics step
	sf::Vector3f force_;
	sf::Vector3f acceleration_;

	// Total Mass of the body
	float mass_;


	// variables for drawing body to render target
	float modelRadius_;
	sf::Vector3f colour_;

	bool destroy_;
	bool collisionTreeInserted_;

	Body* nextBody_;
};

