#include <vector>
#include "Texture.h"
#include "Plant.h"
#include "Bowl.h"
#include "GameObject.h"
#include "Effect.h"
#include "Bowl.h"

#ifndef PLAYER
#define PLAYER
class Player : public GameObject{
	//insert all of the stuff that the GameObject does not keep track of
public:
	//movement logic variables
	bool falling;

	//stats
	int health;
	int defense;
	int attack;
	float attackSpeed;
	float speed;
	float recovery;
	float luck;

	//type of bowl
	//bowl also stores the textures/animation textures
	Bowl *bowl = nullptr;

	//plants
	int numPlants{};
	int currentPlant{-1}; // -1 represents the basic attack
	Plant *one = nullptr ;
	Plant *two = nullptr;
	Plant *three = nullptr;
	Plant *four = nullptr; //only applicable for one of them

	//if the player is currently on an interactalbe GameObject, this will store it
	GameObject *interact = nullptr;

	//effects
	std::vector<Effect> effects;

	Player(){};
	void loadPlayer(const char *file);
	void applyEffects();
};
#endif