#pragma once


struct CollisionEvent {
	Body* b1;
	Body* b2;
	CollisionEvent* next;

	CollisionEvent(Body* newB1 = nullptr, Body* newB2 = nullptr, CollisionEvent* nextCollision = nullptr) {
		b1 = newB1;
		b2 = newB2;
		next = nextCollision;
	}

	~CollisionEvent() {

		if (next) {

			delete next;
			next = nullptr;
		}
	}
};

