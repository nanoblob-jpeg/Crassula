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
	int numBullet;
	float projectileSpeed;
	bool right{};
	Projectile(){};
	Projectile(glm::vec2 psize, int pdamage, int prange, std::string pname, int pnumBullet, float pprojectileSpeed = 200, bool ppiercing = false)
		: damage{pdamage}, piercing{ppiercing}, range{prange}, name{pname}, projectileSpeed{pprojectileSpeed}, numBullet{pnumBullet}{
			size = psize;
		};

	void setDirection(const glm::vec2 &startPosition, const short direction, float speed){
		if(direction < 0){
			velocity[0] = -speed;
		}else{
			velocity[0] = speed;
		}
		position = startPosition;
	}

	void setDirection(const glm::vec2 &startPosition, const short direction){
		if(direction < 0){
			velocity[0] = -projectileSpeed;
		}else{
			velocity[0] = projectileSpeed;
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