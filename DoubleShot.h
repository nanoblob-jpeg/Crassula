#ifndef DOUBLESHOT
#define DOUBLESHOT

#include "ResourceManager.h"
#include <iostream>
class DoubleShot : public Enemy{
	bool doublefire = true;
public:
	DoubleShot(){
		health = 7;
		defense = 2;
		attack = 4;
		attackSpeed = 0.7;
		speed = 0.5;
		projectileSpeed = 400;
		sprite = (ResourceManager::GetTexture("enemies"));
		projectileName = "doubleShotEnemyProjectile";
		size = glm::vec2(40, 40);
		name = "DoubleShot";
	}

	bool attackFunc(glm::vec3 &playerPosition){
		if(!attacking){
			short distance = playerPosition[0] - posiion[0];
			if(abs(distance) <= 550 && abs(playerPosition[1] - position[1]) < 50){
				attackRight = distance >= 0;
				attacking = true;
				if(doublefire){
					attackingTime = 3 - attackSpeed - 0.2;
					doublefire = false;
				}else{
					doublefire = true;
				}
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
		if(attackRight){
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