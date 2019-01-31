#pragma once
#include <SFML\System\Vector3.hpp>
#include <cmath>
#include "SETTINGS.h"

class Body;

class PhysicsUtil
{
public:
	PhysicsUtil();
	~PhysicsUtil();

	static float DistanceTo(const sf::Vector3f& pos1, const sf::Vector3f& pos2);
	static float DistanceToSqr(const sf::Vector3f& pos1, const sf::Vector3f& pos2);
	static float VectorLengthSqr(const sf::Vector3f& vector);
	static sf::Vector3f VectorBetween(const sf::Vector3f& pos1, const sf::Vector3f& pos2);

	static float Normalise(sf::Vector3f& vector);
	static void AddForcesBetween(Body* body1, Body* body2);

	static const float G;
	static const float pi;
	static const float dampeningFactor;
};

