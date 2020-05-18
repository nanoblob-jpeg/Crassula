#ifndef EFFECT
#define EFFECT
//interface for effects
class Effect{
public:
	virtual void applyEffect(int &health, int &defense, int &attack, float &speed, float &recovery);
};
#endif