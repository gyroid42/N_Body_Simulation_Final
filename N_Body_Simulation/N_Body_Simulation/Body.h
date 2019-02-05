#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

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

	void Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass);

	// Methods to add a force to the body
	void AddForce(sf::Vector3f newForce);
	void AddForce(Body* body);
	void AddForce(sf::Vector3f bodyPos, float bodyMass);

	// Resets the bodies force to 0
	void ResetForce();

	// Integration Methods
	void Integrate_SemiImplicitEuler(float dt);

	// Interpolates between the previous state and current state
	State InterpolateState(float alpha);
	//void Integrate_Rk4(float t, float dt);
	//void Integrate_Verlet();


	// Draws the body to the render target
	void Draw(float alpha);

	
	// Getters
	inline float Mass() { return mass_; }
	inline sf::Vector3f Position() { return currentState_.position_; }
	inline float ModelRadius() { return modelRadius_; }
	inline State CurrentState() { return currentState_; }

	// Setters
	inline void SetColour(sf::Vector3f rgb) { colour_ = rgb; }
	void SetMass(float newMass);

private:

	// State of body in the previous physics step
	State prevState_;

	// State of body in the current physics step
	State currentState_;


	// Force acting on the body this physics step
	sf::Vector3f force_;

	// Total Mass of the body
	float mass_;


	// variables for drawing body to render target
	float modelRadius_;
	sf::Vector3f colour_;

};

