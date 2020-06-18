#include "Texture.h"
#include "Effect.h"
#include "GameObject.h"
#include <glm/glm.hpp>
#include <vector>

#ifndef PROJECTILE
#define PROJECTILE
class Projectile : public GameObject{
public:
	std::string name;
	int damage;
	bool piercing;
	int range;
	float distanceTravelled{};
	std::vector<Effect> effects;
	std::vector<glm::vec2> texturePosition;
	Projectile(){};
	Projectile(glm::vec2 size, Texture sprite, int pdamage, int prange, std::string pname, bool ppiercing = false)
		: GameObject(glm::vec2(0,0), size, sprite), damage{pdamage}, piercing{ppiercing}, range{prange}, name{pname}{};

	void setDirection(const glm::vec2 &startPosition, const short direction){
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

	void addEffect(Effect &effect);

	bool rangeCheck();
};
#endif