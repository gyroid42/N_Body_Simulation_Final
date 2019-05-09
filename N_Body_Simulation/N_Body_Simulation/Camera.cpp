
// class header include
#include "Camera.h"

// standard library includes
#include <math.h>

// glut includes
#include <GL\glut.h>

// my class includes
#include "Input.h"
#include "PhysicsUtil.h"


Camera::Camera()
{
}


Camera::~Camera()
{
}


void Camera::Init(Input* newInput) {

	// get reference to user input
	input_ = newInput;

	// set default values
	pos_ = sf::Vector3f(1000.0f, 0.0f, 0.0f);
	lookAt_ = sf::Vector3f(0.0f, 0.0f, 0.0f);
	up_ = sf::Vector3f(0.0f, 1.0f, 0.0f);
	
	rotation_ = sf::Vector3f(270.0f, 0.0f, 0.0f);

	speed_ = 500.0f;
	rotSpeed_ = 60.0f;

	// Update the view vectors
	UpdateView();
}


void Camera::CleanUp() {


}

void Camera::Update(float frameTime) {


	// input for camera movement
	if (input_->IsKeyDown('w')) {
		Move(FORWARD, frameTime);
	}
	if (input_->IsKeyDown('s')) {
		Move(BACKWARD, frameTime);
	}
	if (input_->IsKeyDown('d')) {
		Move(RIGHT, frameTime);
	}
	if (input_->IsKeyDown('a')) {
		Move(LEFT, frameTime);
	}
	if (input_->IsKeyDown('-')) {
		Move(DOWN, frameTime);
	}
	if (input_->IsKeyDown('=')) {
		Move(UP, frameTime);
	}



	// input for camera rotation
	sf::Vector3f newRotation(0.0f, 0.0f, 0.0f);

	if (input_->IsKeyDown('i')) {
		newRotation.y += 1.0f;
	}
	if (input_->IsKeyDown('k')) {
		newRotation.y -= 1.0f;
	}
	if (input_->IsKeyDown('j')) {
		newRotation.x -= 1.0f;
	}
	if (input_->IsKeyDown('l')) {
		newRotation.x += 1.0f;
	}

	if (PhysicsUtil::VectorLengthSqr(newRotation) > 0.0f) {
		Rotate(newRotation, frameTime);
	}

}

void Camera::UpdateView() {

	// temp values for sin/cos
	float cosR, cosP, cosY;
	float sinR, sinP, sinY;

	//calculate sin/cos values from rotation
	cosY = cosf(rotation_.x*3.1415f / 180.0f);
	cosP = cosf(rotation_.y*3.1415f / 180.0f);
	cosR = cosf(rotation_.z*3.1415f / 180.0f);
	sinY = sinf(rotation_.x*3.1415f / 180.0f);
	sinP = sinf(rotation_.y*3.1415f / 180.0f);
	sinR = sinf(rotation_.z*3.1415f / 180.0f);

	//calculate forward vector
	forward_.x = sinY * cosP;
	forward_.y = sinP;
	forward_.z = cosP * -cosY;

	//calculate up vector
	up_.x = -cosY * sinR - sinY * sinP*cosR;
	up_.y = cosP * cosR;
	up_.z = -sinY * sinR - sinP * cosR*(-cosY);

	//calculate right vector
	right_.x = forward_.y*up_.z - forward_.z*up_.y;
	right_.y = forward_.z*up_.x - forward_.x*up_.z;
	right_.z = forward_.x*up_.y - forward_.y*up_.x;
	PhysicsUtil::Normalise(right_);

	//set lookat position
	lookAt_ = pos_ + forward_;

}


void Camera::Move(MOVE_DIRECTION direction, float frameTime) {

	// translate position in direction
	switch (direction) {
	case FORWARD:
		pos_ += forward_ * speed_ * frameTime;
		break;
	case BACKWARD:
		pos_ -= forward_ * speed_ * frameTime;
		break;
	case RIGHT:
		pos_ += right_ * speed_ * frameTime;
		break;
	case LEFT:
		pos_ -= right_ * speed_ * frameTime;
		break;
	case UP:
		pos_.y += speed_ * frameTime;
		break;
	case DOWN:
		pos_.y -= speed_ * frameTime;
		break;
	}

	// update lookat
	lookAt_ = pos_ + forward_;
}

void Camera::Rotate(sf::Vector3f direction, float frameTime) {

	// update rotation vector
	rotation_ += direction * frameTime * rotSpeed_;

	// update view vectors
	UpdateView();

}



void Camera::SetGluLookAt() {

	gluLookAt(pos_.x, pos_.y, pos_.z,
		lookAt_.x, lookAt_.y, lookAt_.z,
		up_.x, up_.y, up_.z);
}
