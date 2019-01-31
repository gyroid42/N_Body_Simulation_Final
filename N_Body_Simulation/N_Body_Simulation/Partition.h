#pragma once

#include <SFML\System\Vector3.hpp>
class Partition
{
public:
	Partition();
	Partition(sf::Vector3f newPosition, float newLength);
	~Partition();


	Partition GetSubDivision(int octantNum);

	bool Contains(sf::Vector3f point);

	inline float Length() { return length_; }

private:

	sf::Vector3f position_;
	float length_;

};

