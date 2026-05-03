#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
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
	int gridPos[3] = { 0, 0, 0 };
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
	bool isCube = true;
	glm::vec3 color = glm::vec3(0.1f, 0.1f, 0.1f);
	std::shared_ptr<Mesh> mesh;
	Shader* shader;
	Camera* camera;


	Entity(std::shared_ptr<Mesh> meshP, Shader* s, Camera* c, float xCoord = 0, float yCoord = 0, float zCoord = 0)
	{
		mesh = meshP;
		shader = s;
		camera = c;
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

	void scale(float scale)
	{
		sizeX *= scale;
		sizeY *= scale;
		sizeZ *= scale;

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

	void moveHorizontal(float x_movement) {
		translate(x_movement, 0, 0);
		std::cout << "Paddle moved to x: " << x << std::endl;
	}

	void Draw() {
		shader->use();
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
		
		shader->setMat4("view", view);
		shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(x, y, z));
		model = glm::scale(model, glm::vec3(sizeX, sizeY, sizeZ));
		shader->setMat4("model", model);

		shader->setVec3("objectColor", color);
		shader->setBool("isCube", isCube);

		glBindVertexArray(mesh->vaoId);
		glDrawArrays(GL_TRIANGLES, 0, 36);

	}
	
};