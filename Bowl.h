#ifndef BOWL
#define BOWL
#include <vector>
#include "Texture.h"

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
	float attackSpeed;
	float speed;
	float recovery;
	float luck;

	int width;
	int height;

	Bowl(int h, int d, int a, float as, float s, float r, float l, int w, int he):
	health{h}, defense{d}, attack{a}, attackSpeed{as}, speed{s}, recovery{r}, luck{l}, width{w}, height{he}{};
};
#endif