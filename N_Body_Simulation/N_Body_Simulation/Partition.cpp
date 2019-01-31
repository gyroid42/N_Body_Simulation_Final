#include "Partition.h"



Partition::Partition() {

	position_ = sf::Vector3f(0.0f, 0.0f, 0.0f);
	length_ = 10000.0f;
}

Partition::Partition(sf::Vector3f newPosition, float newLength)
{
	position_ = newPosition;
	length_ = newLength;
}


Partition::~Partition()
{
}



Partition Partition::GetSubDivision(int octantNum) {

	if (octantNum >= 8) {
		return Partition(position_, length_);
	}

	int x = octantNum % 2;
	int y = (octantNum >> 1) % 2;
	int z = (octantNum >> 2) % 2;

	return Partition(sf::Vector3f(position_.x + length_ * (2.0f * (float)x - 1.0f) / 4.0f, position_.y + length_ * (2.0f * (float)y - 1.0f) / 4.0f, position_.z + length_ * (2.0f * (float)z - 1.0f) / 4.0f), length_ / 2.0f);
}


bool Partition::Contains(sf::Vector3f point) {

	if (point.x >= position_.x - length_ / 2.0f && point.x <= position_.x + length_ / 2.0f &&
		point.y >= position_.y - length_ / 2.0f && point.y <= position_.y + length_ / 2.0f &&
		point.z >= position_.z - length_ / 2.0f && point.z <= position_.z + length_ / 2.0f) {

		return true;
	}

	return false;
}
