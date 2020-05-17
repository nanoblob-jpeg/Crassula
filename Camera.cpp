#include "Camera.h"

glm::mat4 Camera::GetViewMatrix(){
	return glm::lookAt(Position, Position + Front, Up);
};
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime){
    float temp = Position.y;
    float velocity = movementSpeed * deltaTime;
    if (direction == LEFT){
        Position -= Right * velocity;
        Position.y = temp;
    }
    if (direction == RIGHT){
        Position += Right * velocity;
        Position.y = temp;
    }
    if (direction == UP){
        Position += Up * velocity;
    }
    if (direction == DOWN){
        Position -= Up * velocity;
    }
};

void Camera::ProcessKeyboard(glm::vec2 movement_vector, float deltaTime){
    Position += Right * movement_vector.x * deltaTime;
    Position += Up * movement_vecotr.y * deltaTime;
    Position.z = 1;
}
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch){
	xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
};

void Camera::updateCameraVectors(){
    Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
};