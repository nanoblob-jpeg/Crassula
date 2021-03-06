#ifndef BOWL
#define BOWL
#include <vector>
#include "Texture.h"
#include <glm/glm.hpp>
#include <string>

class Bowl{
public:
	std::string name;
	//animation Textures
	//Texture 0 will be the normal walking/idle photo
	std::vector<Texture> attackAnimation;
	//animation frame counters
	int frameCounter{};
	float frameTimer{};
	bool inAnimation{false};

	//(de)buffs
	int health;
	int defense;
	int attack;
	int numOfPlants;
	int numOfLevelFour;
	float attackSpeed;
	float speed;
	float recovery;
	float luck;

	glm::vec2 size;
	Bowl(){};
	Bowl(std::string &n, int h, int d, int a, float as, float s, float r, float l, int nop, int nolf, glm::vec2 psize):
	name{n}, health{h}, defense{d}, attack{a}, attackSpeed{as}, speed{s}, recovery{r}, luck{l}, numOfPlants{nop}, numOfLevelFour{nolf}, size(psize){};

	void startAnimationCounter(){
		frameCounter = 0;
		frameTimer = 0;
		inAnimation = true;
	}

	void addFrameTimer(const float dt){
		frameTimer += dt;
		frameCounter = static_cast<int>(frameTimer / 0.16);
		if(frameCounter == attackAnimation.size()){
			inAnimation = false;
			frameCounter = 0;
		}
	}

	float getSizeX(){
		return size[0];
	}

	float getSizeY(){
		return size[1];
	}

	int getNumPlants(){
		return numOfPlants;
	}

	glm::vec2 getSize(){
		return size;
	}

	bool getInAnimation(){
		return inAnimation;
	}

	Texture& getTexture(){
		return attackAnimation[frameCounter];
	}

	int getNumOfLevelFour(){
		return numOfLevelFour;
	}
};
#endif