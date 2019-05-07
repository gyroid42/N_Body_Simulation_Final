// Below ifdef required to remove warnings for unsafe version of fopen and fscanf.
// Secure version won't work cross-platform, forcing this small hack.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "model.h"

#include "PhysicsUtil.h"

Model::Model() {
}

Model::~Model() {
}

bool Model::load(char* modelFilename, char* textureFilename)
{
	bool result;

	// Load in the model data,
	result = loadModel(modelFilename);
	if (!result)
	{
		MessageBox(NULL, "Model failed to load", "Error", MB_OK);
		return false;
	}

	// Load the texture for this model.
	//loadTexture(textureFilename);
	isTriangles = true;
	return true;
}

void Model::render()
{
	// TODO: Need to add code here to render the loaded model
	// How this is done is based on how you stored and sorted the data
	glPushMatrix();
		
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(angle, rotDirection.x, rotDirection.y, rotDirection.z);
		glScalef(scale.x, scale.y, scale.z);
		glColor3f(colour.x, colour.y, colour.z);
		
		glBindTexture(GL_TEXTURE_2D, texture);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glNormalPointer(GL_FLOAT, 0, normals.data());
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords.data());
		if (isTriangles) {
			glVertexPointer(3, GL_FLOAT, 0, vertex.data());
			glDrawArrays(GL_TRIANGLES, 0, m_vertexCount / 3);
		}
		else {
			glVertexPointer(3, GL_FLOAT, 0, vertex.data());
			glDrawArrays(GL_QUADS, 0, m_vertexCount / 3);
		}
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glPopMatrix();
}


// Modified from a mulit-threaded version by Mark Ropper.
bool Model::loadModel(char* filename)
{
	vector<sf::Vector3f> verts;
	vector<sf::Vector3f> norms;
	vector<sf::Vector3f> texCs;
	vector<unsigned int> faces;

	FILE* file = fopen(filename, "r");
	if (file == NULL)
	{
		return false;
	}
	while (true)
	{
		char lineHeader[128];

		// Read first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
		{
			break; // exit loop
		}
		else // Parse
		{
			if (strcmp(lineHeader, "v") == 0) // Vertex
			{
				sf::Vector3f vertex;
				fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				verts.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) // Tex Coord
			{
				sf::Vector3f uv;
				fscanf(file, "%f %f\n", &uv.x, &uv.y);
				texCs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) // Normal
			{
				sf::Vector3f normal;
				fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				norms.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) // Face
			{
				unsigned int face[9];
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &face[0], &face[1], &face[2],
					&face[3], &face[4], &face[5],
					&face[6], &face[7], &face[8]);
				if (matches != 9)
				{
					// Parser error, or not triangle faces
					return false;
				}

				for (int i = 0; i < 9; i++)
				{
					faces.push_back(face[i]);
				}


			}
		}
	}

	// "Unroll" the loaded obj information into a list of triangles.
	// TODO: By this point all model has been read from the file, but is not in the correct order.
	// You NEED to loop over all the data and sort it into a render ready order/format.



	for (int i = 0; i < faces.size(); i+=3) {
		vertex.push_back(verts[faces[i] - 1].x);
		vertex.push_back(verts[faces[i] - 1].y);
		vertex.push_back(verts[faces[i] - 1].z);

		texCoords.push_back(texCs[faces[i + 1] - 1].x);
		texCoords.push_back(texCs[faces[i + 1] - 1].y);

		normals.push_back(norms[faces[i + 2] - 1].x);
		normals.push_back(norms[faces[i + 2] - 1].y);
		normals.push_back(norms[faces[i + 2] - 1].z);
	}

	m_vertexCount = vertex.size();

	// Once data has been sorted clear read data (which has been copied and are not longer needed).
	verts.clear();
	norms.clear();
	texCs.clear();
	faces.clear();

	return true;
}


sf::Vector3f Model::getScale() {
	return scale;
}

sf::Vector3f Model::getPos() {
	return pos;
}

sf::Vector3f Model::getRotDirection() {
	return rotDirection;
}

float Model::getAngle() {
	return angle;
}

void Model::setScale(float x, float y, float z) {
	scale = { x, y, z };
	return;
}

void Model::setPos(float x, float y, float z) {
	pos = { x, y, z };
	return;
}

void Model::setPos(sf::Vector3f newPos) {
	pos = newPos;
	return;
}

void Model::setRot(float newAngle, float x, float y, float z) {
	rotDirection = { x, y, z };
	PhysicsUtil::Normalise(rotDirection);
	angle = newAngle;
	return;
}

void Model::setColour(sf::Vector3f newColour) {

	colour = newColour;
	return;
}

void Model::pushVert(float x, float y, float z) {
	vertex.push_back(x);
	vertex.push_back(y);
	vertex.push_back(z);
	m_vertexCount += 3;
	return;
}
void Model::pushNorm(float x, float y, float z) {
	normals.push_back(x);
	normals.push_back(y);
	normals.push_back(z);
	return;
}
void Model::pushTexC(float u, float v) {
	texCoords.push_back(u);
	texCoords.push_back(v);
	return;
}

void Model::setIsTriangles(bool tri) {
	isTriangles = tri;
}



void Model::GenerateSphere(float longitude, float latitude, float r) {

	float theta = (2 * 3.1415) / latitude;
	float delta = 3.1415 / longitude;
	float x = 0;
	float y = 0;
	float z = 0;
	float u = 1 / latitude;
	float v = 1 / longitude;
	vertex.clear();
	normals.clear();
	texCoords.clear();

	for (int i = 0; i < longitude; i++) {
		for (int j = 0; j < latitude; j++) {
			x = r * cosf(theta*j)*sinf(delta*i);
			y = r * cosf(delta*i);
			z = r * sinf(theta*j)*sinf(delta*i);
			pushTexC(u*j, v*i);
			pushNorm(x / r, y / r, z / r);
			pushVert(x, y, z);


			x = r * cosf(theta*j)*sinf(delta*(i + 1));
			y = r * cosf(delta*(i + 1));
			z = r * sinf(theta*j)*sinf(delta*(i + 1));
			pushTexC(u*j, v*(i + 1));
			pushNorm(x / r, y / r, z / r);
			pushVert(x, y, z);

			x = r * cosf(theta*(j + 1))*sinf(delta*(i + 1));
			y = r * cosf(delta*(i + 1));
			z = r * sinf(theta*(j + 1))*sinf(delta*(i + 1));
			pushTexC(u*(j + 1), v*(i + 1));
			pushNorm(x / r, y / r, z / r);
			pushVert(x, y, z);

			x = r * cosf(theta*(j + 1))*sinf(delta*i);
			y = r * cosf(delta*i);
			z = r * sinf(theta*(j + 1))*sinf(delta*i);
			pushTexC(u*(j + 1), v*i);
			pushNorm(x / r, y / r, z / r);
			pushVert(x, y, z);
		}
	}
	setIsTriangles(false);
	setPos(0, 0, 0);
	setRot(0, 0, 0, 1);
}

