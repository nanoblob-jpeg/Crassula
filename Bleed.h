#ifndef BLEED
#define BLEED
class Bleed : public Effect {
public:
	int damage;
	Bleed(int pdamage):damage(pdamage){};

	void applyEffects(int &health, int &defense, int &attack, float &speed, float &recovery){
		health -= damage;
	}
};
#endif