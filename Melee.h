#ifndef MELEE
#define MELEE

#include "ResourceManager.h"
#include <iostream>
class Melee : public Enemy{
public:
	Melee(){
		health = 5;
		defense = 4;
		attack = 3;
		attackSpeed = 0.5;
		speed = 0.5;
		projectileSpeed = 180;
		sprite = (ResourceManager::GetTexture("enemies"));
		glm::vec2 texCoord = glm::vec2(0.0, 0.0);
		projectileName = "meleeEnemyProjectile";
		for(int i{}; i < 1; ++i){
			std::vector<glm::vec2> tempVec;
			int temp = 40;
			unsigned int texWidth = sprite.m_width;
			unsigned int texHeight = sprite.m_height;
				
			glm::vec2 tempCoord;
			tempCoord[0] = (texCoord[0]/texWidth);
			tempCoord[1] = ((texCoord[1] + temp)/texHeight);
			tempVec.push_back(tempCoord);

			tempCoord[0] = ((texCoord[0] + temp)/texWidth);
			tempCoord[1] = (texCoord[1]/texHeight);
			tempVec.push_back(tempCoord);

			tempCoord[0] = (texCoord[0]/texWidth);
			tempCoord[1] = (texCoord[1]/texHeight);
			tempVec.push_back(tempCoord);

			tempVec.push_back(tempVec[0]);

			tempCoord[0] = ((texCoord[0] + temp)/texWidth);
			tempCoord[1] = ((texCoord[1] + temp)/texHeight);
			tempVec.push_back(tempCoord);

			tempVec.push_back(tempVec[1]);
			texturePosition.push_back(tempVec);
		}
		size = glm::vec2(40, 40);
		name = "Melee";
	}
	bool attackFunc(glm::vec3 &playerPosition){
		if(!attacking){
			short distance = playerPosition[0] - position[0];
			if(abs(distance) <= 500){
				if(distance < 0){
					attackRight = false;
				}else{
					attackRight = true;
				}
				attacking = true;
				return true;
			}else{
				return false;
			}
		}else{
			return false;
		}
	}
	void move(){
		position[0] += 0.01;
	}
	glm::vec2 getProjectileStartPositionForEnemy(Projectile &p){
		glm::vec2 output;
		if(attackRight){
			output[0] = position[0] + size[0];
		}else{
			output[0] = position[0] - p.size[0];
		}
		output[1] = position[1] - (size[1] - p.size[1])/2;
		return output;
	}
};
#endif