#pragma once

#include <list>
#include "SETTINGS.h"


class Body;
class Renderer;

namespace sf {

	//class CircleShape;
}


class Simulation
{
public:
	Simulation();
	virtual ~Simulation();

	virtual void Init();

	virtual void CleanUp();

	virtual bool GenerateAsteroids(int numAsteroids);

	virtual bool Reset();

	virtual void TimeStep(float dt);

	virtual void Render(float alpha);



	virtual void AddBody(Body* newBody);

protected:

	void CleanUpBodies();

	int bodyCount_;

	std::list<Body*> bodies_;

};

