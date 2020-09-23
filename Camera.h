#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera values
const float SPEED       =  2.5f;

class Camera{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;


	float movementSpeed;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)):Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed{SPEED}{
		Position = position;
        WorldUp = up;

		updateCameraVectors();
	};
	// Camera(float posX, float posY, float posZ, float upX, float upY, float upZ) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED)
 //    {
 //        Position = glm::vec3(posX, posY, posZ);
 //        WorldUp = glm::vec3(upX, upY, upZ);

 //        updateCameraVectors();
 //    }
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessKeyboard(const glm::vec2 &movement_vector, const float deltaTime);
private:
	void updateCameraVectors();
};
#endif