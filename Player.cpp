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
		std::getline(fstream, line);
		size[0] = std::stof(line);
		std::getline(fstream, line);
		size[1] = std::stof(line);
	}else{
		std::cout << "Player file could not be opened";
	}
}

void Player::applyEffects(){
	for(int i{}; i < effects.size(); ++i){
		effects[i].applyEffects(health, defense, attack, speed, recovery);
	}
}