#include "Player.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "ResourceManager.h"

void Player::loadPlayer(const char *file){
	/*
	information in the file is going to be formatted like this:
	name
	level
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
		std::getline(fstream, line);
		name = line;
		std::getline(fstream, line);
		level = std::stoi(line);
		std::getline(fstream, line);
		experience = std::stof(line);
		calculateStats(level);
		std::getline(fstream, line);
		bowl = ResourceManager::GetBowl(line);
	}else{
		std::cout << "Player file could not be opened";
	}
}

void Player::applyEffects(){
	for(int i{}; i < effects.size(); ++i){
		effects[i].applyEffect(health, defense, attack, speed, recovery);
	}
}

void Player::calculateStats(int level){
	health = level * 2 + 5;
	defense = level * 0.25 + 2;
	attack = level * 0.5 + 1;
	attackSpeed = level * 0.02 + 0.5;
	speed = level * 0.2 + 1.0;
	recovery = level * 0.1 + 0.1;
	luck = level * 0.01 + 1;
}