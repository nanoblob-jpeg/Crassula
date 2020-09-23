#ifndef HEAVY
#define HEAVY

#include "ResourceManager.h"
#include <iostream>
class Heavy : public Enemy{
public:
	Heavy(){
		health = 15;
		defense = 7;
		attack = 8;
		attackSpeed = -2;
		speed = 0.5;
		projectilesSpeed = 50;
		sprite = ResourceManager::GetTexture("enemies");
		projectileName = "heavyEnemyProjectile";
		size = glm::vec2(40, 40);
		name = "Heavy";
	}
	bool attackFunc(glm::vec3 &playerPosition){
		if(!attacking){
			short distance = playerPosition[0] - position[0];
			if(abs(distance) <= 400 && abs(playerPosition[1] - position[1]) < 40){
				attackRight = distance >= 0;
				attacking = true;
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}
	glm::vec2 getProjectileStartPositionForEnemy(Projectile &p){
		glm::vec2 output;
		if(attackingRight){
			output[0] = position[0] + size[0];
		}else{
			output[0] = position[0] - p.size[0];
		}
		output[1] = position[1] - (size[1] - p.size[1])/2;
		for(int i{}; i < attackEffects.size(); ++i){
			p.effects.push_back(attackEffects[i]);
		}
		return output;
	}
};
#endif