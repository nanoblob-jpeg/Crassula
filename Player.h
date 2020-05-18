#include <vector>
#include "Texture.h"
#include "Plant.h"
#include "Bowl.h"
#include "GameObject.h"
#include "Effect.h"

#ifndef PLAYER
#define PLAYER
class Player : public GameObject{
	//insert all of the stuff that the GameObject does not keep track of
public:
	bool falling;
	int health;
	int defense;
	int attack;
	float attackSpeed;
	float speed;
	float recovery
	float luck;

	//animation Textures
	std::vector<Texture> attackAnimation;
	//animation frame counters
	int frameCounter;

	//type of bowl
	Bowl bowl;

	//plants
	int numPlants{};
	int currentPlant{-1}; // -1 represents the basic attack
	Plant one;
	Plant two;
	Plant three;
	Plant four; //only applicable for one of them

	//if the player is currently on an interactalbe GameObject, this will store it
	GameObject *interact;

	//effects
	std::vector<Effect> effects;

	Player(){};
	void loadPlayer(const char *file);
	void applyEffects();
};
#endif