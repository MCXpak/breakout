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
#include <random>

//BUG LIST
// 1. Scaling to 0? How to deal? Should it be allowed ( I think so ?)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void checkCollision(Entity& a, Entity& b);
void updateEntities(std::vector<Entity>& entityList, std::vector<std::vector<std::vector<int>>>& grid, Shader& shader, ResourceManager& manager);
void processPaddleInput(GLFWwindow* window, Entity& paddle);

Camera camera(glm::vec3(3.0f, 15.0f, 20.0f));

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float lastX = 400, lastY = 300;
bool firstMouse = true; // initialize true so first mouse callback sets lastX/lastY
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<double> distr(-0.01, 0.01);

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

std::vector<float> plane_vertices = {
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

std::vector<std::vector<std::vector<int>>> grid = { 
{
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}
}, 
{
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}
}
    };

std::vector<glm::vec3> colorOptions = {
    glm::vec3(1.0f, 0.2f, 0.2f),
    glm::vec3(0.2f, 1.0f, 0.2f),
    glm::vec3(0.2f, 0.2f, 1.0f),
    glm::vec3(1.0f, 1.0f, 0.2f),
    glm::vec3(1.0f, 0.2f, 1.0f),
    glm::vec3(0.2f, 1.0f, 1.0f)
};

std::vector<float> quadVertices = {
    // positions     // colors
    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  1.0f, 0.0f, 0.0f,
    -0.05f, -0.05f,  1.0f, 0.0f, 0.0f,

    -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
     0.05f, -0.05f,  1.0f, 0.0f, 0.0f,
     0.05f,  0.05f,  1.0f, 0.0f, 0.0f
};

std::vector<Entity> particles;

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
    Shader particleShader("./particleShader.vs", "./particleShader.fs");
    
    // projection can be calculated once (perspective parameters are constant)
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    // Upload projection to shaders that will use it (do once)
    shader.use();
    shader.setMat4("projection", projection);
    lightObjectShader.use();
    lightObjectShader.setMat4("projection", projection);
    particleShader.use();
    particleShader.setMat4("projection", projection);

    std::vector<Entity> entities;
    std::vector<Entity> bricks;

    // Create plane
	Entity plane(manager.createMesh("plane", plane_vertices, indices, 6), &shader, &camera, 2.5f, -1.0f, 7.0f);
	plane.scale(22.0f, 1.0f, 15.0f);
	plane.type = 2;
	plane.isCube = false;
	plane.color = glm::vec3(0.2f, 0.2f, 0.2f);

    // Create ball (use float literals to avoid double->float truncation warnings)
    Entity ball(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, 11.0f);
    ball.velX = 0.0003f;
    ball.velZ = 0.0003f;
    ball.scale(0.8f);
	ball.color = glm::vec3(1.0f, 1.0f, 1.0f);
    entities.push_back(ball);
    
	Entity paddle(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 3.0f, 0.0f, 13.0f);
	paddle.scale(3.0f, 0.5f, 1.0f);
	paddle.type = 2;
	paddle.color = glm::vec3(0.8f, 0.8f, 0.8f);
	entities.push_back(paddle);

    // Create grid of breakout bricks (cast indices to float)
    for (int i = 0; i < static_cast<int>(grid.size()); i++) {
        for (int j = 0; j < static_cast<int>(grid[i].size()); j++) {
            for (int k = 0; k < static_cast<int>(grid[i][j].size()); k++) {
                if (grid[i][j][k] == 1) {
                    Entity ent(manager.createMesh("cube", vertices, indices, 36), &shader, &camera,
                               (float) (k + BOUNDARY_LEFT + 0.5), (float) i, (float) (j));
                    ent.color = colorOptions[j];
					ent.gridPos[0] = i;
					ent.gridPos[1] = j;
					ent.gridPos[2] = k;
                    ent.scale(2.0f, 1.0f, 1.0f);
                    entities.push_back(ent);
                }
            }
        }
	};

    Entity wallRight(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 13.0f, 0.0f, 7.0f);
    wallRight.scale(1.0f, 1.0f, 15.0f);
    wallRight.type = 2;
    entities.push_back(wallRight);

    Entity wallLeft(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, -8.0f, 0.0f, 7.0f);
    wallLeft.scale(1.0f, 1.0f, 15.0f);
    wallLeft.type = 2;
    entities.push_back(wallLeft);

    Entity wallTop(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 2.5f, 0.0f, -1.0f);
    wallTop.scale(22.0f, 1.0f, 1.0f);
    wallTop.type = 2;
    entities.push_back(wallTop);

    Entity wallBottom(manager.createMesh("cube", vertices, indices, 36), &shader, &camera, 2.5f, 0.0f, 15.0f);
    wallBottom.scale(22.0f, 1.0f, 1.0f);
    wallBottom.type = 2;
    entities.push_back(wallBottom);

    Entity lightCube(manager.createMesh("cube", vertices, indices, 36), &lightObjectShader, &camera, 0.0f, 7.0f, 0.0f);

    //Entity particles(manager.createMesh("particle", quadVertices, translations), &particleShader, &camera, 0.0f, 0.0f, 0.0f, translations.size());

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

        // Update view each frame to reflect camera movement
        glm::mat4 view = camera.GetViewMatrix();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Main shader: update view and camera position
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
		plane.Draw();

        for (Entity &e : entities) {
            checkCollision(entities[0], e);

            updateEntities(entities, grid, particleShader, manager);

			e.Draw();

			// Update point light position to follow ball
			pointLightPositions[1] = glm::vec3(entities[0].x, entities[0].y, entities[0].z);

        }

        // ensure light object's view/projection are set (projection set once above)
        lightCube.Draw();

        for (Entity &particle : particles) {
            particle.DrawInstanced();
		}

        double currentFrame = glfwGetTime();
        deltaTime = static_cast<float>(currentFrame - static_cast<double>(lastFrame));
        lastFrame = static_cast<float>(currentFrame);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}

void generateParticles(Entity &ent, Shader &particleShader, ResourceManager &manager) {
    // Number of particles to generate for each explosion
    const int PARTICLE_COUNT = 64;

    // Create offsets and velocities for each particle
    std::vector<glm::vec2> instanceOffsets;
    std::vector<glm::vec2> instanceVels;

    for (int i = 0; i < PARTICLE_COUNT; ++i) {
        // Small position jitter around the entity's X,Y to spread particles
        float offsetX = (float) (distr(gen));
        float offsetY = (float) (distr(gen));
        instanceOffsets.emplace_back(offsetX, offsetY);

        // Random initial velocity for each particle
        instanceVels.emplace_back((float) (distr(gen)), (float) (distr(gen)));
    }

    // Only create the particle entity if we have offsets
    if (!instanceOffsets.empty()) {
        Entity particleExplosion( manager.createMesh("particle", quadVertices, instanceOffsets), &particleShader, 
            &camera, ent.x, ent.y, ent.z, (int) (instanceOffsets.size())
        );
		particleExplosion.color = ent.color;
		std::cout << "Generated " << instanceOffsets.size() << " particles for explosion at (" << ent.x << ", " << ent.y << ", " << ent.z << ")" << std::endl;

        // Attach instance data if Entity supports it (original code indicated these members)
        particleExplosion.instanceOffsets = instanceOffsets;
        particleExplosion.instanceVels = instanceVels;

        particles.push_back(particleExplosion);
    }
}

void updateEntities(std::vector<Entity>& entityList, std::vector<std::vector<std::vector<int>>>& grid, Shader& shader, ResourceManager& manager) {
    //Find indexes to "disappear" and/or update
    
    for (Entity& e : entityList) {
        if (e.type == 3){
            generateParticles(e, shader, manager);

            e.translate(99, 99, 99);
            e.type = 0;
			grid[e.gridPos[0]][e.gridPos[1]][e.gridPos[2]] = 0;
        }
		// Check if there is nothing below current entity, if so, apply gravity
        if (e.gridPos[0] > 0 && grid[e.gridPos[0] - 1][e.gridPos[1]][e.gridPos[2]] == 0) {
            e.velY = -0.0001f;
        }
        e.update();
    }
}

void checkCollision(Entity &a, Entity &b)
//Currently only calcuating on XZ plane (horizontal)
{
    if (a.id != b.id && b.y <= 0.0f) {
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
            }
        }
        
    }
    if (b.y <= 0.0f) {
		b.y = 0.0f;
		b.velY = 0.0f;
		b.updateBoundingBox();
    }
    ;
    
    
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