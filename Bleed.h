#ifndef BLEED
#define BLEED
#include "Effect.h"
class Bleed : public Effect {
public:
	int damage;
	Bleed(int pdamage):damage(pdamage){};

	void applyEffects(int &health, int &defense, int &attack, float &speed, float &recovery){
		health -= damage;
	}
};
#endif