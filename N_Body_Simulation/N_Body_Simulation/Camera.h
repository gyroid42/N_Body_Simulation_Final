#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

// forward declarations
class Input;


class Camera
{

	// enum for user camera input
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

	// Update called each frame
	void Update(float frameTime);

	// Update View called each time the forward, right and up vectors need calculated
	void UpdateView();

	// methods for moving/rotating the camera
	void Move(MOVE_DIRECTION direction, float frameTime);
	void Rotate(sf::Vector3f direction, float frameTime);


	// Updates glut camera
	void SetGluLookAt();

private:

	// vectors to construct view matrix
	sf::Vector3f pos_;
	sf::Vector3f lookAt_;
	sf::Vector3f up_;
	sf::Vector3f forward_;
	sf::Vector3f right_;
	sf::Vector3f rotation_;

	// controls the speed the camera moves/rotates
	float speed_;
	float rotSpeed_;

	// reference to input to get user input
	Input* input_;
};

