#ifndef EFFECT
#define EFFECT
//interface for effects
#include <string>
class Effect{
public:
	std::string name;
	//variables for applying effects
	int dehealth{};
	int dedefense{};
	int deattack{};
	float despeed{};
	float derecovery{};

	//logic for ones that just have a constant debuff
	bool once{};
	bool apply{true};

	//counts number of times this activates
	float time{};
	//counts the time that has passed
	float timeCounter{};
	//this is how often it does it
	float frameTimeCounter{};
	//will activate once more after it surpasses this
	/*
	ex:
	time = 3;
	frameTimeCounter = 20;
	timeCounter goes to t - frameTimeCounter after it runs
	 */
	Effect(){};
	Effect(std::string &pname, int dh, int dd, int da, float ds, float dr, float ptime, float pframeTimeCounter, bool ponce = false):
		name{pname}, dehealth{dh}, dedefense{dd}, deattack{da}, despeed{ds}, derecovery{dr}, time{ptime}, frameTimeCounter{pframeTimeCounter}, once{ponce}{};

	//false if it can still run
	//true if it can't run anymore
	bool applyEffect(int &health, int &defense, int &attack, float &speed, float &recovery, const float dt){
		timeCounter += dt;
		if(once && apply){
			health += dehealth;
			defense += dedefense;
			attack += deattack;
			speed += despeed;
			recovery += derecovery;
			apply = false;
			return false;
		}else if(timeCounter >= frameTimeCounter){
			health += dehealth;
			defense += dedefense;
			attack += deattack;
			speed += despeed;
			recovery += derecovery;
			timeCounter -= frameTimeCounter;
			if(!(--time)){
				if(once){
					health -= dehealth;
					defense -= dedefense;
					attack -= deattack;
					speed -= despeed;
					recovery -= derecovery;
				}
				return true;
			}
			else
				return false;
		}else{
			return false;
		}
	};
};
#endif