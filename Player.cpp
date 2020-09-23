#include "Player.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ResourceManager.h"
#include <numeric>

void Player::loadPlayer(const char *file){
	/*
	information in the file is going to be formatted like this:
	name
	level
	backgroundName
	experience
	bowl name
	*/
	//player doesn't start with any plants in the home page so there is no plant loading happening here
	//player starts where the camera starts, so we don't need a position
		//we can just set the position equal to the camera's position but in the 0 z axis
	velocity[0] = 0;
	velocity[1] = 0;
	std::string line;
	std::ifstream fstream(file);
	if(fstream){
		std::getline(fstream, name);
		std::getline(fstream, line);
		level = std::stoi(line);
		std::getline(fstream, backgroundName);
		std::getline(fstream, line);
		experience = std::stof(line);
		calculateStats();
		std::getline(fstream, line);
		bowl = &(ResourceManager::GetBowl(line).first);
	}else{
		std::cout << "Player file could not be opened";
	}
}

void Player::applyEffects(float dt){
	for(int i{}; i < effects.size(); ++i){
		if(effects[i].applyEffect(health, defense, attack, speed, recovery, dt)){
			effects.erase(effects.begin() + i);
			--i;
		}
	}
}

void Player::calculateStats(){
	health = level * 2 + 5;
	defense = level * 0.25 + 2;
	attack = level * 0.5 + 1;
	attackSpeed = std::max(0.5, std::min(level * 0.02 + 0.5, 2.9));
	speed = level * 0.2 + 1.0;
	recovery = level * 0.1 + 0.1;
	luck = level * 0.01 + 1;
}

void Player::switchPlant(bool direction){
	/*
	direction = true is to the right
	direction = false is to the left
	*/
	if(currentPlant != -1){
		if(direction){
			if(bowl->numOfPlants == 3){
				currentPlant = (currentPlant + 1)%std::min(numPlants, 3);
			}else if(bowl->numOfPlants == 4){
				currentPlant = (currentPlant + 1)%std::min(numPlants, 4);
			}
		}else{
			if(bowl->numOfPlants == 3){
				currentPlant = currentPlant - 1 < 0 ? numPlants-1 : currentPlant-1;
			}else if(bowl->numOfPlants == 4){
				currentPlant = currentPlant - 1 < 0 ? numPlants-1 : currentPlant-1;
			}
		}
	}
}

void Player::dealDamage(int damage){
	health -= std::max(damage - defense, 1);
}

bool Player::isDead(){
	return health <= 0;
}

void Player::addEffects(Projectile &p){
	bool newEffect = true;
	int oldEffectPlace{};
	for(int i{}; i < p.effects.size(); ++i){
		for(int j{}; j < effects.size(); ++j){
			if(effects[j].name.compare(p.effects[i].name) == 0){
				newEffect = false;
				oldEffectPlace = j;
				break;
			}
		}
		if(newEffect){
			this->effects.push_back(p.effects[i]);
		}
		else{
			this->effects.erase(effects.begin() + oldEffectPlace);
			this->effects.push_back(p.effects[i]);
			newEffect = true;
		}
	}
}

bool Player::canAttack(float dt){
	if(timer < 0.0001){
		timer += dt;
		return true;
	}
	timer += dt;
	if(timer > 3 - (attackSpeed))
		timer = 0;
	return false;
}

void Player::calculateLevel(){
	while(experience >= (level+1) * (level+1)){
		level += 1;
		experience -= (level) * (level);
	}
}

void Player::setStatBoosts(){
	statBoosts.push_back(bowl->health);
	statBoosts.push_back(bowl->defense);
	statBoosts.push_back(bowl->attack);
	statBoosts.push_back(bowl->attackSpeed);
	statBoosts.push_back(bowl->speed);
	statBoosts.push_back(bowl->recovery);
	statBoosts.push_back(bowl->luck);
}

void Player::setFinalStats(){
	health = 1 > health + statBoosts[0] ? 1 : health + statBoosts[0];
	defense += statBoosts[1];
	attack += statBoosts[2];
	attackSpeed = 0.5 > attackSpeed + statBoosts[3] ? 0.5 : attackSpeed + statBoosts[3];
	speed = 1.0 > speed + statBoosts[4] ? 1.0 : speed + statBoosts[4];
	recovery = 0.0 > recovery + statBoosts[5] ? 0.0 : recovery + statBoosts[5];
	luck = 1.0 > luck + statBoosts[6] ? 1.0 : luck+statBoosts[6];
}

void Player::setStatBoosts(std::vector<float> &p, float level){

	statBoosts[0] += p[0] * level;
	statBoosts[1] += p[1] * level;
	statBoosts[2] += p[2] * level;
	statBoosts[3] += p[3] * level;
	statBoosts[4] += p[4] * level;
	statBoosts[5] += p[5] * level;
	statBoosts[6] += p[6] * level;
}

std::string& Player::getBackgroundName(){
	return backgroundName;
}

void Player::startAnimationCounter(){
	bowl->startAnimationCounter();
}

void Player::addFrameTimer(float dt){
	bowl->addFrameTimer(dt);
}

bool Player::inAnimation(){
	return bowl->inAnimation;
}

std::string Player::getBowlName(){
	return bowl->name;
}

void Player::setBowl(Bowl *b){
	bowl = b;
}

float Player::getSpeed(){
	return speed;
}

int Player::getAttack(){
	return attack;
}

void Player::recoveryProc(int maxHealth){
	health += recovery;
	health = std::min(health, maxHealth);
}

int Player::getHealth(){
	return health;
}

float Player::getLuck(){
	return luck;
}

int Player::getLevel(){
	return level;
}

void Player::addExperience(float a){
	experience += a;
}

float Player::getExperience(){
	return experience;
}

std::string Player::getName(){
	return name;
}

float Player::getSizeX(){
	return bowl->getSizeX();
}

float Player::getSizeY(){
	return bowl->getSizeY();
}

int Player::getMaxNumPlants(){
	return bowl->getNumPlants();
}

glm::vec2 Player::getSize(){
	return bowl->getSize();
}

void Player::incrementAnimation(float dt){
	if(bowl->getInAnimation()){
		bowl->addFrameTimer(dt);
	}
}

Texture& Player::getTexture(){
	return bowl->getTexture();
}

bool Player::getFalling(){
	return falling;
}

void Player::setFalling(bool b){
	falling = b;
}

bool Player::getSwitchingPlants(){
	return switchingPlants;
}

void Player::setSwitchingPlants(bool b){
	switchingPlants = b;
}

int Player::getNumPlants(){
	return numPlants;
}

int Player::getNumCurrentLevelFourPlants(){
	return numCurrentLevelFourPlants;
}

int Player::getMaxNumOfLevelFour(){
	return bowl->getNumOfLevelFour();
}

void Player::addToNumCurrentLevelFourPlants(int i){
	numCurrentLevelFourPlants += i;
}

Plant* Player::getPlant(int i){
	return &plants[i];
}

void Player::addPlant(std::string name, bool code){
	//code is true when plant is to be pushed back
	//false when plant replaces current plant
	if(code){
		plants.push_back(ResourceManager::GetPlant(name));
	}else{
		plants[currentPlant] = ResourceManager::GetPlant(name);
	}
}

void Player::setCurrentPlant(int a){
	currentPlant = a;
}

void Player::addNumPlants(int i){
	numPlants += i;
}

Plant* Player::getCurrentPlant(){
	return getPlant(currentPlant);
}

std::string Player::getProjectileName(){
	return plants[currentPlant].getProjectileName();
}

int Player::getCurrentPlantNum(){
	return currentNum;
}

void Player::clearPlants(){
	plants.clear();
}

std::vector<Plant>::iterator Player::getPlantArrayBegin(){
	return plants.begin();
}

std::vector<Plant>::iterator Player::getPlantArrayEnd(){
	return plants.end();
}