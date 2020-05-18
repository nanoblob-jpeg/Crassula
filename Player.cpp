#include "Player.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ResourceManager.h"

void Player::loadPlayer(const char *file){
	/*
	information in the file is going to be formatted like this:
	health
	defense
	attack
	attackSpeed
	speed
	recovery
	luck
	bowl name
	Texture name
	width
	height
	*/
	//player doesn't start with any plants in the home page so there is no plant loading happening here
	//player starts where the camera starts, so we don't need a position
		//we can just set the position equal to the camera's position but in the 0 z axis
	
	//todo change this entire parsing part
	//the stats of the player depend on the bowl that they are using and the buffs
	//that are in effect at the time
	//don't know if we should store the buffs active at the time in order to
	//instantiate these or if we should just apply the buffs whenever they enter a new
	//game, i'm leaning a bit more towards storing the buffs
	//need an ending line then, cause they won't always have 9 buffs active
	//boolean vector system for which buffs are active?
	//should probably first think about all of the different buffs that a player can have
	//
	//from the plants in the green house
	//achievements?
	//they should level, so maybe this isn't bad, but maybe change all of this to be
	//calculated off of their level
	//i will just make the bowl add or subtract from their health/other stats when using them for
	//calculations
	//then with the greenhouse plants, instead of them increasing by percentage, have them increase
	//by int, and then do the same thing as with the bowls and add or subtract them 
	//when you need to do calculations with them
	std::string line;
	std::ifstream fstream(file);
	if(fstream){
		std::getline(fstream, line);
		health = std::stoi(line);
		std::getline(fstream, line);
		defense = std::stoi(line);
		std::getline(fstream, line);
		attack = std::stoi(line);
		std::getline(fstream, line);
		attackSpeed = std::stof(line);
		std::getline(fstream, line);
		speed = std::stof(line);
		std::getline(fstream, line);
		recovery = std::stof(line);
		std::getline(fstream, line);
		luck = std::stof(line);
		std::getline(fstream, line);
		bowl = ResourceManager::GetBowl(line);
		std::getline(fstream, line);
		sprite = ResourceManager::GetTexture(line.c_str());
		//size comes from the bowl that is being used
		//don't need this, don't need to set the dimensions in
		//the gameobject parent class as we already have them in the bowl class
		// std::getline(fstream, line);
		// size[0] = std::stof(line);
		// std::getline(fstream, line);
		// size[1] = std::stof(line);
	}else{
		std::cout << "Player file could not be opened";
	}
}

void Player::applyEffects(){
	for(int i{}; i < effects.size(); ++i){
		effects[i].applyEffect(health, defense, attack, speed, recovery);
	}
}