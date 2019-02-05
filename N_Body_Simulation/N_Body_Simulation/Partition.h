#pragma once

// SFML includes
#include <SFML\System\Vector3.hpp>


class Partition
{
public:
	Partition();
	Partition(sf::Vector3f newPosition, float newLength);
	~Partition();


	// Creates a subDivision of a ceratin octant this partition contains
	Partition GetSubDivision(int octantNum);

	// returns whether point is inside this partition
	bool Contains(sf::Vector3f point);

	// Getters
	inline float Length() { return length_; }

private:

	// position of partition (point is in center)
	sf::Vector3f position_;

	// length is the size of 1 side of the partition
	float length_;
};

