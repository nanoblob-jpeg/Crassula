#include <vector>
#include "Texture.h"
#include "Bowl.h"
#include "GameObject.h"
#include "Effect.h"
#include "Plant.h"

#ifndef PLAYER
#define PLAYER
class Player : public GameObject{
	//insert all of the stuff that the GameObject does not keep track of
public:
	//input logic variables
	bool falling{};
	bool switchingPlants{};

	//general information
	int level = 0;
	float experience= 0;
	std::string name = "";
	std::string backgroundName = "";
	//false for left, true for right
	bool facing;
	float isHit{};

	//stats
	int health = 0;
	int defense = 0;
	int attack = 0;
	float attackSpeed = 0;
	float speed = 0;
	float recovery = 0;
	float luck = 0;

	//type of bowl
	//bowl also stores the textures/animation textures
	Bowl *bowl = nullptr;

	//plants
	int numPlants = 0;
	int currentPlant{-1}; // -1 represents the basic attack
	int numCurrentLevelFourPlants = 0;
	std::vector<Plant> plants = {};

	//if the player is currently on an interactalbe GameObject, this will store it
	//the interact key is currently "i"
	glm::vec4 location{0.0,0.0,0.0, 0.0};
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

	//attacking logic
	float timer = 0;

	Player():GameObject(){};
	void loadPlayer(const char *file);
	void applyEffects(float dt);
	void calculateStats();
	void switchPlant(bool direction);
	void dealDamage(int damage);
	bool isDead();
	void addEffects(Projectile &p);
	bool canAttack(float dt);
	void calculateLevel();
	void setStatBoosts();
	void setStatBoosts(std::vector<float> &p, float level);
	void setFinalStats();
private:
	int getDefenseBoost();
	int getHealthBoost();
};
#endif