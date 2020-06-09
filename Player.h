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

	//general information
	int level;
	float experience;
	std::string name;
	std::string backgroundName;
	//false for left, true for right
	bool facing;

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
	int numCurrentLevelFourPlants{};
	std::vector<Plant *> plants{nullptr, nullptr, nullptr, nullptr};

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

	//attacking logic
	float timer{};

	Player(){};
	void loadPlayer(const char *file);
	void applyEffects(float dt);
	void calculateStats();
	void switchPlant();
	void dealDamage(int damage);
	bool isDead();
	void addEffects(Projectile &p);
	bool canAttack();
	void calculateLevel();
private:
	int getDefenseBoost();
	int getHealthBoost();
};
#endif