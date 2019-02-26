
// class header include
#include "Partition.h"



Partition::Partition() {

	position_ = sf::Vector3f(0.0f, 0.0f, 0.0f);
	halfLength_ = 5000.0f;
}

Partition::Partition(sf::Vector3f newPosition, float newLength)
{
	position_ = newPosition;
	halfLength_ = newLength;
}


Partition::~Partition()
{
}



Partition Partition::GetSubDivision(int octantNum) {

	// if number is above 8 then outside of array
	if (octantNum >= 8) {
		return Partition(position_, halfLength_);
	}

	// get x, y and z co-ordinate of octant
	//int x = ((octantNum & 1) ? 1 : 0);
	//int y = ((octantNum & 2) ? 1 : 0);
	//int z = ((octantNum & 4) ? 1 : 0);

	float step = halfLength_ * 0.5f;

	sf::Vector3f offset;

	offset.x = ((octantNum & 1) ? step : -step);
	offset.y = ((octantNum & 2) ? step : -step);
	offset.z = ((octantNum & 4) ? step : -step);

	// create new partition using co-ordinates and length of current partition
	return Partition(position_ + offset, step);
	//return Partition(sf::Vector3f(position_.x + length_ * (2.0f * (float)x - 1.0f) / 4.0f, position_.y + length_ * (2.0f * (float)y - 1.0f) / 4.0f, position_.z + length_ * (2.0f * (float)z - 1.0f) / 4.0f), length_ / 2.0f);
}


bool Partition::Contains(sf::Vector3f point) {

	// checks if a point is within the partition
	if (point.x >= position_.x - halfLength_ && point.x <= position_.x + halfLength_ &&
		point.y >= position_.y - halfLength_ && point.y <= position_.y + halfLength_ &&
		point.z >= position_.z - halfLength_ && point.z <= position_.z + halfLength_) {

		return true;
	}

	return false;
}
