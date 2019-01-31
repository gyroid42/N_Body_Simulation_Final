#pragma once

#include <SFML\System\Vector3.hpp>


class Renderer;


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


	inline State operator*(float a) {

		
		return { position_ * a, velocity_ * a };
	}

	inline State operator+(const State& a) {
		//position_ += a.position_;
		//position_ += a.velocity_;
		return { position_ + a.position_, velocity_ + a.velocity_ };
	}

};


class Body
{
public:
	Body();
	~Body();

	void Init(sf::Vector3f newPos, sf::Vector3f newVel, float newMass);

	//void AddForce(Body* b);
	void AddForce(sf::Vector3f newForce);
	void ResetForce();
	void Integrate_SemiImplicitEuler(float dt);
	State InterpolateState(float alpha);
	void Draw(Renderer* renderer, float alpha);
	//void Integrate_Rk4(float t, float dt);
	//void Integrate_Verlet();

	//void SetColour(sf::Color newColor);

	inline float Mass() { return mass_; }
	inline State CurrentState() { return currentState_; }
	//inline sf::CircleShape* Sprite() { return sprite_; }

private:

	State prevState_;
	State currentState_;

	sf::Vector3f force_;
	float mass_;

	float modelRadius_;

	//sf::CircleShape* sprite_;

};

