
// class header include
#include "PhysicsUtil.h"

// my class includes
#include "Body.h"

PhysicsUtil::PhysicsUtil()
{


}


PhysicsUtil::~PhysicsUtil()
{
}


float PhysicsUtil::DistanceTo(const sf::Vector3f& pos1, const sf::Vector3f& pos2) {

	return sqrtf(DistanceToSqr(pos1, pos2));
}

float PhysicsUtil::DistanceToSqr(const sf::Vector3f& pos1, const sf::Vector3f& pos2) {

	float dX = pos1.x - pos2.x;
	float dY = pos1.y - pos2.y;
	float dZ = pos1.z - pos2.z;

	return dX*dX + dY*dY + dZ*dZ;
}

float PhysicsUtil::VectorLength(const sf::Vector3f& vector) {

	return sqrtf(VectorLengthSqr(vector));
}

float PhysicsUtil::VectorLengthSqr(const sf::Vector3f& vector) {

	return vector.x*vector.x + vector.y*vector.y + vector.z*vector.z;
}


sf::Vector3f PhysicsUtil::VectorBetween(const sf::Vector3f& pos1, const sf::Vector3f& pos2) {

	return pos2 - pos1;
}

float PhysicsUtil::Normalise(sf::Vector3f& vector) {

	float distance = sqrt(VectorLengthSqr(vector));

	vector = vector / distance;

	return distance;
}


void PhysicsUtil::AddForcesBetween(Body* body1, Body* body2) {


	sf::Vector3f distanceVector = PhysicsUtil::VectorBetween(body1->Position(), body2->Position());
	float distance = PhysicsUtil::Normalise(distanceVector);
	distance += dampeningFactor;

	sf::Vector3f force = (PhysicsUtil::G * body1->Mass() * body2->Mass() / (distance * distance)) * distanceVector;

	body1->AddForce(force);
	//body2->AddForce(force);
}


const float PhysicsUtil::G = 6.673E-11f;
const float PhysicsUtil::pi = 3.14159f;
const float PhysicsUtil::dampeningFactor = DAMPENING_FACTOR;
