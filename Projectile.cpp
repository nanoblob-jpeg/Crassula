#include "Projectile.h"
void Projectile::setDirection(const glm::vec2 &startPosition, const short direction, float speed){
	if(direction < 0){
		velocity[0] = -speed;
	}else{
		velocity[0] = speed;
	}
	position = startPosition;
}

void Projectile::setDirection(const glm::vec2 &startPosition, const short direction){
	if(direction < 0){
		velocity[0] = -projectileSpeed;
	}else{
		velocity[0] = projectileSpeed;
	}
	position = startPosition;
}

void Projectile::setDirectionY(){
	velocity[1] = projectileSpeed;
}

void Projectile::move(float dt){
	position[0] += velocity[0] * dt;
	distanceTravelled += velocity[0] * dt;
	if(velocity[1] != 0){
		position[1] += velocity[1] * dt;
	}
}

void Projectile::addEffect(Effect &effect){
	effects.push_back(effect);
}

bool Projectile::rangeCheck(){
	return abs(distanceTravelled) >= range;
}