#pragma once

#include <SFML\System\Vector3.hpp>

class Input;

class Camera
{
	enum MOVE_DIRECTION {
		FORWARD,
		BACKWARD,
		RIGHT,
		LEFT,
		UP,
		DOWN
	};
public:
	Camera();
	~Camera();

	void Init(Input* newInput);
	void CleanUp();

	void Update(float frameTime);
	void UpdateView();

	void Move(MOVE_DIRECTION direction, float frameTime);
	void Rotate(sf::Vector3f direction, float frameTime);

	void SetGluLookAt();

private:

	sf::Vector3f pos_;
	sf::Vector3f lookAt_;
	sf::Vector3f up_;
	sf::Vector3f forward_;
	sf::Vector3f right_;
	sf::Vector3f rotation_;

	float speed_;
	float rotSpeed_;

	Input* input_;
};

