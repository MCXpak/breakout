#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "camera.h"
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

Camera camera(glm::vec3(3.0f, 10.0f, 3.0f));

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = false;
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

std::vector<float> vertices = {
    // Position (3)  | Color (3)     | Tex Coords (2)
    // --- Face 1: -Z (Front) ---
    -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,

    // --- Face 2: +Z (Back) ---
    -0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,

    // --- Face 3: -X (Left) ---
    -0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,

    // --- Face 4: +X (Right) ---
     0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,

     // --- Face 5: -Y (Bottom) ---
     -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,

     // --- Face 6: +Y (Top) ---
     -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,   0.5f, 0.0f, 0.0f,   0.0f, 1.0f
};

std::vector<int> indices = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
};

std::vector<std::vector<int>> grid = {
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 1}
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


    Shader shader("./shader.vs", "./shader.fs");

    ResourceManager manager;

    std::vector<Entity> entities;
    std::vector<Entity> bricks;

    // Create ball
    Entity ball(manager.createMesh("cube", vertices, indices, 36), 3, 0, 11);
    ball.velX = 0.001;
    ball.velZ = 0.001;
    entities.push_back(ball);

    // Create grid of breakout bricks
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            if (grid[i][j] == 1) {
                Entity ent(manager.createMesh("cube", vertices, indices, 36), j, 0, i);
                entities.push_back(ent);
            }
        }
    };

    Entity wallRight(manager.createMesh("cube", vertices, indices, 36), 7, 0, 7);
    wallRight.scale(1, 1, 15);
    wallRight.type = 2;
    entities.push_back(wallRight);

    Entity wallLeft(manager.createMesh("cube", vertices, indices, 36), -1, 0, 7);
    wallLeft.scale(1, 1, 15);
    wallLeft.type = 2;
    entities.push_back(wallLeft);

    Entity wallTop(manager.createMesh("cube", vertices, indices, 36), 3, 0, -1);
    wallTop.scale(9, 1, 1);
    wallTop.type = 2;
    entities.push_back(wallTop);

    Entity wallBottom(manager.createMesh("cube", vertices, indices, 36), 3, 0, 15);
    wallBottom.scale(9, 1, 1);
    wallBottom.type = 2;
    entities.push_back(wallBottom);

    shader.use();

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    int viewLoc = glGetUniformLocation(shader.ID, "view");
    int projectionLoc = glGetUniformLocation(shader.ID, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        shader.use();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = camera.GetViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // Check collision against cube ball vs other entities
        for (Entity &e : entities) {
            glBindVertexArray(e.mesh->vaoId);

            checkCollision(entities[0], e);

            updateEntities(entities);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(e.x, e.y, e.z));
            model = glm::scale(model, glm::vec3(e.sizeX, e.sizeY, e.sizeZ));
            
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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

void static checkCollision(Entity &a, Entity &b)
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