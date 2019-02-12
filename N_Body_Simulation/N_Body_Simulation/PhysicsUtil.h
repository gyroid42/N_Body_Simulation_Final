#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>

// standard library includes
#include <cmath>

// my class includes
#include "SETTINGS.h"

// forward declarations
class Body;

class PhysicsUtil
{
public:
	PhysicsUtil();
	~PhysicsUtil();

	// Vector Maths
	static float DistanceTo(const sf::Vector3f& pos1, const sf::Vector3f& pos2);
	static float DistanceToSqr(const sf::Vector3f& pos1, const sf::Vector3f& pos2);
	static float VectorLength(const sf::Vector3f& vector);
	static float VectorLengthSqr(const sf::Vector3f& vector);
	static sf::Vector3f VectorBetween(const sf::Vector3f& pos1, const sf::Vector3f& pos2);
	static float Normalise(sf::Vector3f& vector);

	// Adds the force acting on body1 due to body2
	static void AddForcesBetween(Body* body1, Body* body2);

	// Physics constants
	static const float G;
	static const float pi;
	static const float dampeningFactor;
};

