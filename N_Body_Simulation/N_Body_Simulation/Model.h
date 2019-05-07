// Model class, represents a loaded OBJ model
// handles the loading/processing of the model file. Also loads and stores its own texture
// NOT complete, for students to complete. Model data requires being sorted and the render function needs completed.
#ifndef _MODEL_H_
#define _MODEL_H_

// INCLUDES //
#include <GL\glut.h>
#include <fstream>
#include <gl/gl.h>
#include <gl/glu.h>
#include <SFML/System/Vector3.hpp>

using namespace std;

// MY CLASS INCLUDES //
#include <vector>


class Model
{

public:

	Model();
	~Model();

	bool load(char* modelFilename, char* textureFilename);
	void render();

	sf::Vector3f getScale();
	sf::Vector3f getPos();
	sf::Vector3f getRotDirection();
	float getAngle();

	void setScale(float x, float y, float z);
	void setPos(float x, float y, float z);
	void setPos(sf::Vector3f newPos);
	void setRot(float angle, float x, float y, float z);
	void setColour(sf::Vector3f newColour);

	void pushVert(float x, float y, float z);
	void pushNorm(float x, float y, float z);
	void pushTexC(float u, float v);

	void setIsTriangles(bool tri);


	void GenerateSphere(float longitude, float latitude, float r);

private:

	
	bool loadModel(char*);

	bool isTriangles;
	int m_vertexCount = 0;
	GLuint texture;
	sf::Vector3f colour;

	sf::Vector3f scale = { 1, 1, 1 };
	sf::Vector3f pos = { 0, 0, 0 };
	sf::Vector3f rotDirection = { 0, 0, 1 };
	float angle = 0;

	vector<float> vertex, normals, texCoords;

};

#endif