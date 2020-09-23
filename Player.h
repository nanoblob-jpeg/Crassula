#include <vector>
#include "Texture.h"
#include "Bowl.h"
#include "GameObject.h"
#include "Effect.h"
#include "Plant.h"

#ifndef PLAYER
#define PLAYER
class Player : public GameObject{
private:
	//false for left, true for right
	bool facing;
	float isHit{};

	//input logic variables
	bool falling{};
	bool switchingPlants{};

	//general information
	std::string backgroundName = "";
	std::string name = "";
	int level = 0;
	float experience= 0;

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

	//attacking logic
	float timer = 0;

	//stat boost vector
	//instantiate when the player enters the gate and starts a game
	//add together all of the greenhouse plant buffs and the bowl buff
	//then when we're calculating, just change the base stats based on
	//these values which should be easy to access
	//maybe change it to a map
	std::vector<float> statBoosts;
public:
	//if the player is currently on an interactalbe GameObject, this will store it
	//the interact key is currently "i"
	glm::vec4 location{0.0,0.0,0.0, 0.0};
	GameObject *interact = nullptr;

	//effects
	std::vector<Effect> effects;

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

	void startAnimationCounter();
	void addFrameTimer(float dt);
	void recoveryProc(int maxHealth);
	void addExperience(float a);
	void incrementAnimation(float dt);
	void addToNumCurrentLevelFourPlants(int i);
	void addPlant(std::string name, bool a);
	void clearPlants();
	void addNumPlants(int i);
	void decrementIsHit(float dt);
	void clearStatBoosts();

	void setBowl(Bowl *b);
	void setFalling(bool b);
	void setSwitchingPlants(bool b);
	void setCurrentPlant(int a);
	void setFacing(bool b);
	void setIsHit(float a);

	bool inAnimation();
	bool getFalling();
	bool getSwitchingPlants();
	bool getFacing();
	int getAttack();
	int getHealth();
	int getLevel();
	int getMaxNumPlants();
	int getNumPlants();
	int getNumCurrentLevelFourPlants();
	int getMaxNumOfLevelFour();
	int getCurrentPlantNum();
	float getSpeed();
	float getLuck();
	float getExperience();
	float getSizeX();
	float getSizeY();
	float getIsHit();
	float getTimer();
	Plant* getPlant(int i);
	Plant* getCurrentPlant();
	Texture& getTexture();
	std::string& getBackgroundName();
	glm::vec2 getSize();
	std::string getBowlName();
	std::string getName();
	std::string getProjectileName();
	std::vector<Plant>::iterator getPlantArrayBegin();
	std::vector<Plant>::iterator getPlantArrayEnd();
private:
	int getDefenseBoost();
	int getHealthBoost();
};
#endif