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

	void setDirection(const glm::vec2 &startPosition, const short direction, float speed);
	void setDirection(const glm::vec2 &startPosition, const short direction);
	void setDirectionY();
	void move(float dt);
	void addEffect(Effect &effect);
	bool rangeCheck();
};
#endif