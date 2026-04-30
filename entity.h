#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include "mesh.h"

class Entity
{
public:
	int id = rand() % 10000;
	int type = 1;
	float x = 0;
	float y = 0;
	float z = 0;
	float sizeX = 1;
	float sizeY = 1;
	float sizeZ = 1;
	float minX = x - sizeX;
	float maxX = x + sizeX;
	float minY = y - sizeY;
	float maxY = y + sizeY;
	float minZ = z - sizeZ;
	float maxZ = z + sizeZ;
	float velX = 0;
	float velY = 0;
	float velZ = 0;
	std::shared_ptr<Mesh> mesh;


	Entity(std::shared_ptr<Mesh> meshP, float xCoord = 0, float yCoord = 0, float zCoord = 0)
	{
		mesh = meshP;
		x = xCoord;
		y = yCoord;
		z = zCoord;

		updateBoundingBox();
	};

	void update() {
		//Update entity with it's velocity properties
		translate(velX, velY, velZ);
	}

	void translate(float xCoord, float yCoord, float zCoord)
	{
		x = x + xCoord;
		y = y + yCoord;
		z = z + zCoord;

		updateBoundingBox();
	}

	void scale(float xScale, float yScale, float zScale)
	{
		sizeX *= xScale;
		sizeY *= yScale;
		sizeZ *= zScale;

		updateBoundingBox();
	}

	void updateBoundingBox()
	{
		minX = x - sizeX/2;
		maxX = x + sizeX/2;
		minY = y - sizeY/2;
		maxY = y + sizeY/2;
		minZ = z - sizeZ/2;
		maxZ = z + sizeZ/2;
	}
	
};