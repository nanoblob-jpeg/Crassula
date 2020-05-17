#define GLFW_DLL
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "Game.h"
#include "stb_image.h"
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//call back functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
//game object
Game Crassula(SCR_WIDTH, SCR_HEIGHT);
//texture mixing
float alpha{0.0f};

//frame timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
//mouse stuff
float lastX = SCR_WIDTH/2, lastY = SCR_HEIGHT/2;
bool firstMouse{true};

//light source location
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
	//stbi_set_flip_vertically_on_load(true);
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

	Crassula.Init();

	
	/*
	CONFIGS


	 */
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
    	}else{
    		timeTracker = 0;
    		frameCounter = 0;
    	}

    	processInput(window);

    	glfwPollEvents();

        // input
        // -----
        Crassula.ProcessInput(deltaTime);

        Crassula.Update(deltaTime);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
		Crassula.Render();

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
void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
       glfwSetWindowShouldClose(window, true);
    

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Crassula.Keys[GLFW_KEY_W] = true;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Crassula.Keys[GLFW_KEY_S] = true;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Crassula.Keys[GLFW_KEY_A] = true;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Crassula.Keys[GLFW_KAY_D] = true;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


/**
 * coordinates the moving of the mouse, not needed for 2D game unless we want them to be able to click and aim
 * which honestly is not a bad idea
 * @param window tracks mouse within this window
 * @param xpos   new x position of the mouse
 * @param ypos   new y position of the mouse
 */
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	//translate to go into the Game class

	// if(firstMouse){
	// 	lastX = xpos;
	// 	lastY = ypos;
	// 	firstMouse = false;
	// }
	// cam.ProcessMouseMovement(xpos - lastX, lastY - ypos, true);
	// lastX = xpos;
	// lastY = ypos;
}

