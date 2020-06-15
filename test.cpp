#define GLFW_DLL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "ResourceManager.h"
#include "Shader.h"
#include "SpriteRenderer.h"
#include "Camera.h"
#include "stb_image.h"
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//call back functions

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//texture mixing
float alpha{0.0f};

//frame timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Game *gamePointer;

int main()
{
	stbi_set_flip_vertically_on_load(true);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Crassula", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

	/*
	CONFIGS


	 */
    //game object
    SpriteRenderer *BlockRenderer;
    Camera cam = Camera(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 projection = glm::ortho(-300.0f, 300.0f,
		-400.0f , 400.0f, -1.0f, 1.0f);
	glm::mat4 view = cam.GetViewMatrix();
    ResourceManager::LoadShader("shaders/block_vshader.txt", "shaders/fragShader.txt", nullptr, "block");
    Shader sProgram;
	sProgram = ResourceManager::GetShader("block");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	BlockRenderer = new SpriteRenderer(ResourceManager::GetShader("block"));
	//frame counting variables
	double timeTracker{};
	int frameCounter{};
    // render loop	
    // -----------
    while (!glfwWindowShouldClose(window))
    {
    	float currentFrame = glfwGetTime();
    	deltaTime = currentFrame - lastFrame;
    	lastFrame = currentFrame;

    	//frame counter, debug useful
    	// if(timeTracker < 1){
    	// 	timeTracker += deltaTime;
    	// 	++frameCounter;
    	// }else if(timeTracker>=1){
    	// 	std::cout << frameCounter << '\n';
     //        std::cout << gamePointer->m_state << '\n';
     //        timeTracker = 0;
     //        frameCounter = 0;
    	// }
    	glfwPollEvents();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        std::vector<glm::vec2> offsets;
        for(int i{}; i < 100; ++i){
        	offsets.push_back(glm::vec2(10 * i, 10 * i));
        }
		view = cam.GetViewMatrix();
		BlockRenderer->bindInstanceBuffer(&offsets[0], 100);
		BlockRenderer->setViewMatrix("view", view);
		BlockRenderer->DrawInstancedSprites(100, ResourceManager::GetTexture("block"),
		glm::vec2(0.0f, 0.0f), glm::vec2(50.0f, 50.0f));
	
        //std::cout << gamePointer->cam.Position[0] << "," << gamePointer->cam.Position[1] <<','<< gamePointer->player.velocity[0] << ',' << gamePointer->player.velocity[1]<<'\n';

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
    }

    ResourceManager::Clear();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// sends all keyboard input to the ProcessInput funciton in Game
// -------------------------------------------------------------------------------------------------------