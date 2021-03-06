#include "GameObject.h"


GameObject::GameObject() : position(0.0f, 0.0f), size(1.0f, 1.0f), velocity(0.0f), color(1.0f), rotation(0.0f){};

GameObject::GameObject(glm::vec2 pos, glm::vec2 psize, Texture psprite, glm::vec3 pcolor, glm::vec2 pvelo, bool pinteractable)
	: position(pos), size(psize), velocity(pvelo), color(pcolor), rotation(0.0f), sprite(psprite), interactable(pinteractable){};

int GameObject::interact(Game *game){
	std::cout << "You're not supposed to be doing this, system exiting on crash\n Reason: Tried to interact with uninteractable object\n";
	exit(1);
}

// GameObject& GameObject::operator=(const GameObject &g){
// 	this->position = g.position;
// 	this->size = g.size;
// 	this->velocity = g.velocity;
// 	this->color = g.color;
// 	this->rotation = g.rotation;
// 	this->interactable = g.interactable;
// 	this->type = g.type;
// 	this->sprite = g.sprite;
// 	return *this;
// }