#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "camera.h"
#include "Shader.h"
#define SHADER_H
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "entity.h"
#include "resource_manager.h"

//BUG LIST
// 1. Scaling to 0? How to deal? Should it be allowed ( I think so ?)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void checkCollision(Entity& a, Entity& b);
void updateEntities(std::vector<Entity> &entityList);
void processPaddleInput(GLFWwindow* window, Entity& paddle);

Camera camera(glm::vec3(3.0f, 10.0f, 15.0f));

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

std::vector<float> vertices = {
    // Position (3)  | Normal (3)     | Tex Coords (2)    
    // --- Face 1: -Z (Front) ---
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, -1.0f,   0.0f, 0.0f,

    // --- Face 2: +Z (Back) ---
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,

    // --- Face 3: -X (Left) ---
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

    // --- Face 4: +X (Right) ---
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,

     // --- Face 5: -Y (Bottom) ---
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,   0.0f, 1.0f,

     // --- Face 6: +Y (Top) ---
     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

std::vector<int> indices = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
};

float BOUNDARY_LEFT = -7.0f;
float BOUNDARY_RIGHT = 13.0f;
float BOUNDARY_TOP = -1.0f;
float BOUNDARY_BOTTOM = 15.0f;

std::vector<std::vector<int>> grid = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1}
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    ResourceManager manager;

    Shader shader("./shader.vs", "./shader.fs");
    Shader lightObjectShader("./lightObjectShader.vs", "./lightObjectShader.fs");

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);


    std::vector<Entity> entities;
    std::vector<Entity> bricks;

    // Create ball (use float literals to avoid double->float truncation warnings)
    Entity ball(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, 11.0f);
    ball.velX = 0.001f;
    ball.velZ = 0.001f;
    ball.scale(0.8f);
	ball.color = glm::vec3(1.0f, 1.0f, 1.0f);
    entities.push_back(ball);
    
	Entity paddle(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, 13.0f);
	paddle.scale(3.0f, 0.5f, 1.0f);
	paddle.type = 2;
	entities.push_back(paddle);

    // Create grid of breakout bricks (cast indices to float)
    for (int i = 0; i < static_cast<int>(grid.size()); i++) {
        for (int j = 0; j < static_cast<int>(grid[i].size()); j++) {
            if (grid[i][j] == 1) {
                Entity ent(manager.createMesh("cube", vertices, indices, 36), &shader, &camera,
                           static_cast<float>(j + BOUNDARY_LEFT + 1), 0.0f, static_cast<float>(i));
				ent.color = glm::vec3((std::rand() % 11) / 10.0, (std::rand() % 11) / 10.0, (std::rand() % 11) / 10.0);
                entities.push_back(ent);
            }
        }
    };

    Entity wallRight(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 13.0f, 0.0f, 7.0f);
    wallRight.scale(1.0f, 1.0f, 15.0f);
    wallRight.type = 2;
    entities.push_back(wallRight);

    Entity wallLeft(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, -7.0f, 0.0f, 7.0f);
    wallLeft.scale(1.0f, 1.0f, 15.0f);
    wallLeft.type = 2;
    entities.push_back(wallLeft);

    Entity wallTop(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, -1.0f);
    wallTop.scale(21.0f, 1.0f, 1.0f);
    wallTop.type = 2;
    entities.push_back(wallTop);

    Entity wallBottom(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, 15.0f);
    wallBottom.scale(21.0f, 1.0f, 1.0f);
    wallBottom.type = 2;
    entities.push_back(wallBottom);

    Entity lightCube(manager.createMesh("cube", vertices, indices, 36), &lightObjectShader, &camera, 0.0f, 7.0f, 0.0f);

    shader.use();

    glm::vec3 pointLightPositions[] = {
        glm::vec3(lightCube.x, lightCube.y,  lightCube.z),
        glm::vec3(ball.x, ball.y, ball.z)
    };

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        processPaddleInput(window, entities[1]);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.Position[0], camera.Position[1], camera.Position[2]);

        shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        shader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);
        shader.setVec3("dirLight.specular", 0.05f, 0.05f, 0.05f);
        shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);

        for (int i = 0; i < 2; i++) {
            std::stringstream ss;
            ss << "pointLights[" << i << "]";
            std::string currPointLight = ss.str();
            shader.setVec3(currPointLight + ".position", pointLightPositions[i]);
            shader.setFloat(currPointLight + ".constant", 1.0f);
            shader.setFloat(currPointLight + ".linear", 0.09f);
            shader.setFloat(currPointLight + ".quadratic", 0.032f);
            shader.setVec3(currPointLight + ".ambient", glm::vec3(0.05f));
            shader.setVec3(currPointLight + ".diffuse", glm::vec3(0.5f));
            shader.setVec3(currPointLight + ".specular", glm::vec3(0.5f));
        }

        // Check collision against cube ball vs other entities
        for (Entity &e : entities) {
            checkCollision(entities[0], e);

            updateEntities(entities);

			e.Draw();

			// Update point light position to follow ball
			pointLightPositions[1] = glm::vec3(entities[0].x, entities[0].y, entities[0].z);

        }

        lightCube.Draw();

        double currentFrame = glfwGetTime();
        deltaTime = static_cast<float>(currentFrame - static_cast<double>(lastFrame));
        lastFrame = static_cast<float>(currentFrame);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}

void updateEntities(std::vector<Entity> &entityList) {
    //Find indexes to "disappear" and/or update
    
    for (Entity& e : entityList) {
        if (e.type == 3){
            e.translate(99, 99, 99);
        }
        e.update();
    }
}

void checkCollision(Entity &a, Entity &b)
//Currently only calcuating on XZ plane (horizontal)
{
    if (a.id != b.id) {
        //Check intersect
        if (a.minX <= b.maxX && a.maxX >= b.minX
            && a.minZ <= b.maxZ && a.maxZ >= b.minZ) {

            //calculate overlap on each axis
            // Overlap on X-axis
            float overlapX = 0.0f;
            if (a.maxX > b.minX && a.minX < b.maxX) {
                // Find the smaller of the two overlaps
                overlapX = std::min(a.maxX - b.minX, b.maxX - a.minX);
            }

            // Overlap on Z-axis
            float overlapZ = 0.0f;
            if (a.maxZ > b.minZ && a.minZ < b.maxZ) {
                // Find the smaller of the two overlaps
                overlapZ = std::min(a.maxZ - b.minZ, b.maxZ - a.minZ);
            }

            if (overlapX < overlapZ) {
                // Collision is primarily on the X-axis
                // Apply reflection velocity
                a.velX = -a.velX;

            }
            else if (overlapZ < overlapX) {
                // Collision is primarily on the Z-axis
                // Apply reflection velocity
                a.velZ = -a.velZ;

            }
            else {
                // overlapX == overlapZ, which means a corner-on collision
                a.velX = -a.velX;
                a.velZ = -a.velZ;
            }
            if (b.type == 1) {
                b.type = 3;
                std::cout << "Id " << b.id << "collision with Type " << b.type;
            }
        }
        
    };
    
    
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
}

void processPaddleInput(GLFWwindow* window, Entity& paddle) {
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        paddle.moveHorizontal(-0.1f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        paddle.moveHorizontal(0.1f);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}