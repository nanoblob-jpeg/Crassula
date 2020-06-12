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
		sprite = (ResourceManager::GetTexture("enemies"));
		glm::vec2 texCoord = glm::vec(0.0, 0.0);
		for(int i{}; i < 1; ++i){
			std::vector<glm::vec2> tempVec;
			int temp = std::max(psize[0], psize[1]);
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

			tempVec.push_back(texturePosition[0]);

			tempCoord[0] = ((texCoord[0] + temp)/texWidth);
			tempCoord[1] = ((texCoord[1] + temp)/texHeight);
			tempVec.push_back(tempCoord);

			tempVec.push_back(texturePosition[1]);
			texturePosition.push_back(tempVec);
		}
		size = glm::vec2(40, 40);
	}
	void attackFunc(){
		std::cout << "enemy attacked";
	}
	void move(){
		position[0] += 0.01;
	}
}
#endif