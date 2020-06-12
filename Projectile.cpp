#include "Projectile.h"
void Projectile::addEffect(Effect effect){
	effects.push_back(effect);
}

bool Projectile::rangeCheck(){
	return distanceTravelled >= range;
}