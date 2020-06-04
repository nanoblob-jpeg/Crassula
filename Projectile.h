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
	std::vector<Effect> *effects;
	std::vector<glm::vec2> texturePosition;

	Projectile(glm::vec2 size, Texture sprite, int pdamage, int prange, bool ppiercing = false)
		: GameObject(glm::vec2(0,0), size, sprite), damage{pdamage}, piercing{ppiercing}, range{prange}{
	};

	void setDirection(glm::vec2 startPosition, float direction){
		if(direction < 0){
			velocity[0] = -5;
		}else{
			velocity[0] = 5;
		}
		position = startPosition;
	}

	void move(float dt){
		position[0] += velocity[0] * dt;
	}

	void addEffect(Effect effect);
};
#endif