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
	glm::vec2 velocity;

	//general information
	int level;
	float experience;
	std::string name;

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
	//the interact key is currently "i"
	GameObject *interact = nullptr;

	//effects
	std::vector<Effect> effects;

	//stat boost vector
	//instantiate when the player enters the gate and starts a game
	//add together all of the greenhouse plant buffs and the bowl buff
	//then when we're calculating, just change the base stats based on
	//these values which should be easy to access
	//maybe change it to a map
	std::vector<float> statBoosts;

	Player(){};
	void loadPlayer(const char *file);
	void applyEffects();
	void calculateStats(int level);
};
#endif