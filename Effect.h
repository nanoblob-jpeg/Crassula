#ifndef EFFECT
#define EFFECT
//interface for effects
class Effect{
public:
	//variables for applying effects
	int dehealth;
	int dedefense;
	int deattack;
	float despeed;
	float derecovery;

	Effect(int dh, int dd, int da, float ds, float dr):
		dehealth{dh}, dedefense{dd}, deattack{da}, despeed{ds}, derecovery{dr}{};

	void applyEffect(int &health, int &defense, int &attack, float &speed, float &recovery){
		health += dehalth;
		defense += dedefense;
		attack += deattack;
		speed += despeed;
		recovery += derecovery;
	};
};
#endif