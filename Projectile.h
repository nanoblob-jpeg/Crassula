#include "Texture.h"
#include "Effect.h"
#include "GameObject.h"
#include <glm/glm.hpp>

#ifndef PROJECTILE
#define PROJECTILE
class Projectile : public GameObject{
public:
	int damage;
	bool piercing;
	int range;
	float distanceTravelled{};
	std::vector<Effect> effects;
	std::vector<glm::vec2> texturePosition;

	Projectile(glm::vec2 size, Texture sprite, int pdamage, int prange, glm::vec2 texCoords, bool ppiercing = false)
		: GameObject(glm::vec2(0,0), size, sprite), damage{pdamage}, piercing{ppiercing}, range{prange}{
		int temp = std::max(size[0], size[1]);
		unsigned int texWidth = sprite.m_width;
		unsigned int texHeight = sprite.m_height;
		
		glm::vec2 tempCoord;
		tempCoord[0] = (texCoord[0]/texWidth);
		tempCoord[1] = ((texCoord[1] + temp)/texHeight);
		texturePosition.push_back(tempCoord);

		tempCoord[0] = ((texCoord[0] + temp)/texWidth);
		tempCoord[1] = (texCoord[1]/texHeight);
		texturePosition.push_back(tempCoord);


		tempCoord[0] = (texCoord[0]/texWidth);
		tempCoord[1] = (texCoord[1]/texHeight);
		texturePosition.push_back(tempCoord);

		texturePosition.push_back(texturePosition[0]);

		tempCoord[0] = ((texCoord[0] + temp)/texWidth);
		tempCoord[1] = ((texCoord[1] + temp)/texHeight);
		texturePosition.push_back(tempCoord);

		texturePosition.push_back(texturePosition[1]);
	};

	void setDirection(glm::vec2 startPosition, short direction){
		if(direction < 0){
			velocity[0] = -5;
		}else{
			velocity[0] = 5;
		}
		position = startPosition;
	}

	void move(float dt){
		position[0] += velocity[0] * dt;
		distanceTravelled += velocity[0] * dt;
	}

	void addEffect(Effect effect);

	bool rangeCheck();
};
#endif