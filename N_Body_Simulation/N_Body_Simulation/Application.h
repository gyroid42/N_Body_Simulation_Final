#pragma once

#include <GL\glut.h>
#include <GL\GL.h>
#include <GL\GLU.h>

class Application
{
public:
	Application();
	~Application();


	void Init();
	void CleanUp();


	bool Update(float frameTime);
	bool SimulationStep(float t, float dt);

	bool Render(float alpha);

	void Resize(int w, int h);

protected:



	int width_, height_;
	float fov_, nearPlane_, farPlane_;

};

