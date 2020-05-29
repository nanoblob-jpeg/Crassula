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
	std::vector<Effect> *effects;

	Projectile(glm::vec2 startPosition, glm::vec2 size, float direction, Texture sprite, int pdamage, std::vector<Effect> *effectPointer, bool ppiercing = false)
		: GameObject(startPosition, size, sprite), damage{pdamage}, piercing{ppiercing}{
			if(direction < 0){
				velocity[0] = -5;
			}else{
				velocity[0] = 5;
			}

			effects = effectPointer;
	};
};
#endif