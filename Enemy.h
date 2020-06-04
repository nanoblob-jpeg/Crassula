#include <vector>
#include "GameObject.h"
#include "Effect.h"

#ifndef ENEMY
#define ENEMY
//interface for enemies
class Enemy : public GameObject{
public:
	int health;
	int defense;
	int attack;
	float attackSpeed;
	float speed;

	std::vector<Texture> attackAnimation;
	int frameCounter;

	std::vector<glm::vec2> texturePosition;

	//stores effects that are influencing the enemy
	//gets added in the plant class, happens when collision happens
	std::vector<Effect> effects;

	std::vector<Effect> attackEffects;

	virtual void attackFunc();
	virtual void move();
	void applyEffects(float dt){
		float temp = 0.0f;
		for(int i{}; i < effects.size(); ++i){
			if(effects[i].applyEffect(health, defense, attack, speed, temp, dt)){
				effects.erase(effects.begin() + i);
				--i;
			}
		}
	};
	void applyMultiplier(float multiplier){
		health *= multiplier;
		defense *= multiplier;
		attack *= multiplier;
		attackSpeed *= multiplier;
		speed *= multiplier;
	}
};
#endif