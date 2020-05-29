#ifndef BOWL
#define BOWL
#include <vector>
#include "Texture.h"
#include <glm/glm.hpp>

class Bowl{
public:
	//animation Textures
	//Texture 0 will be the normal walking/idle photo
	std::vector<Texture> attackAnimation;
	//animation frame counters
	int frameCounter;

	//(de)buffs
	int health;
	int defense;
	int attack;
	int numOfPlants;
	float attackSpeed;
	float speed;
	float recovery;
	float luck;

	glm::vec2 size;

	Bowl(int h, int d, int a, float as, float s, float r, float l, int nop, glm::vec2 psize):
	health{h}, defense{d}, attack{a}, attackSpeed{as}, speed{s}, recovery{r}, luck{l}, numOfPlants{nop}, size(psize){};
};
#endif