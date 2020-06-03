#ifndef GAMEOBJECT
#define GAMEOBJECT

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject{
public:
	//-300, 400 is the top left corner
	//300, -400 is the bottom right corner
	//x is horizontal
	//y is vertical
	glm::vec2 position, size, velocity;
	glm::vec3 color;
	float rotation;
	bool interactable{false};

	Texture sprite;

public:
	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 psize, Texture sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f), bool interactable = false);

	virtual void Draw(SpriteRenderer &renderer);
};
#endif