#ifndef SPREADSHOT
#define SPREADSHOT

#include "ResourceManager.h"
#include <iostream>
class Melee : public Enemy{
	int counter{};
public:
	Spreadshot(){
		health = 5;
		defense = 2;
		attack = 4;
		attackSpeed = 2;
		speed = 1;
		projectileSpeed = 300;
		sprite = ResourceManager::GetTexture("enemies");
		projectileName = "spreadshotEnemyProjectile";
		size = glm::vec2(40, 40);
		name = "Spreadshot";
	}
	bool attackFunc(glm::vec3 &playerPosition){
		if(!attacking){
			if(abs(playerPosition[0] - position[0]) < 500 || abs(playerPosition[1] - position[1]) < 500){
				attackRight = true;
				attacking = true;
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}
	void move(float dt){
		return;
	}
	glm::vec2 getProjectileStartPositionForEnemy(Projectile &p){
		glm::vec2 output;
		switch(counter){
			case 0:
				output[0] = position[0] + size[0];
				output[1] = position[1] - (size[1] - p.size[1])/2;
				counter++;
				attackRight = false;
				break;
			case 1:
				output[0] = position[0] - p.size[0];
				output[1] = position[1] - (size[1] - p.size[1])/2;
				counter++;
				break;
			case 2:
				output[0] = position[0] + size[0]/2;
				output[1] = position[1] + p.size[1];
				counter = 0;
				break;
		}
		for(int i{}; i < attackEffects.size(); ++i){
			p.effects.push_back(attackEffects[i]);
		}
		return output;
	}
};
#endif