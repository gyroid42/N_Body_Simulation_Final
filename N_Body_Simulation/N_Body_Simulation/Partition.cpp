
// class header include
#include "Partition.h"

#include <cmath>


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

	float step = halfLength_ * 0.5f;

	sf::Vector3f offset;

	offset.x = ((octantNum & 1) ? step : -step);
	offset.y = ((octantNum & 2) ? step : -step);
	offset.z = ((octantNum & 4) ? step : -step);

	// create new partition using co-ordinates and length of current partition
	return Partition(position_ + offset, step);
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

bool Partition::StraddleCheck(sf::Vector3f point, float radius) {

	float delta[3];
	delta[0] = point.x - position_.x;
	delta[1] = point.y - position_.y;
	delta[2] = point.z - position_.z;

	for (int i = 0; i < 3; i++) {

		if (abs(delta[i]) <= radius) {

			return true;
		}
	}

	return false;
}
