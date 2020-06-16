#define GLFW_DLL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Game.h"
#include "stb_image.h"
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//call back functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, int key, int scancode, int action, int mode);

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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, processInput);
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
    Game Crassula(SCR_WIDTH, SCR_HEIGHT);
    gamePointer = &Crassula;
    Crassula.Init();

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
    	if(timeTracker < 1){
    		timeTracker += deltaTime;
    		++frameCounter;
    	}else if(timeTracker>=1){
    		std::cout << frameCounter << '\n';
            std::cout << glGetError() << std::endl;
            timeTracker = 0;
            frameCounter = 0;
    	}
    	glfwPollEvents();

        // input
        // -----
        Crassula.ProcessInput(deltaTime);

        Crassula.Update(deltaTime);

        glClearColor(50.0f, 50.0f, 50.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		Crassula.Render();
        std::cout << gamePointer->cam.Position[0] << "," << gamePointer->cam.Position[1] <<','<< gamePointer->player.velocity[0] << ',' << gamePointer->player.velocity[1]<<'\n';

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
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
       glfwSetWindowShouldClose(window, true);
    if(key >= 0 && key <1024){
        if(action == GLFW_PRESS)
            gamePointer->Keys[key] = true;
        else if(action == GLFW_RELEASE)
            gamePointer->Keys[key] = false;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    //prevent the window from becoming a larger size
    glViewport(0, 0, 600, 800);
}