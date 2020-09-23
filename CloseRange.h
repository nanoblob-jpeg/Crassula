#ifndef CLOSERANGE
#define CLOSERANGE

#include "ResourceManager.h"
#include <iostream>
class CloseRange : public Enemy{
public:
	CloseRange(){
		health = 10;
		defense = 5;
		attack = 2;
		attackSpeed = 0.75;
		projectileSpeed = 200;
		sprite = (ResourceManager::GetTexture("enemies"));
		projectileName = "closeRangeEnemyProjectile";
		size = glm::vec2(40, 40);
		name = "CloseRange";
	}

	bool attackFunc(glm::vec3 &playerPosition){
		if(!attacking){
			short distance = playerPosition[0] - position[0];
			if(abs(distance) <= 80 && abs(playerPosition[1] - position[1] < 20)){
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