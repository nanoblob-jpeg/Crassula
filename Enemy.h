#include <vector>
#include "GameObject.h"
#include "Effect.h"
#include "Projectile.h"

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
	std::string name;
	std::string projectileName;

	bool attacking{false};
	float attackingTimer{};
	bool attackRight{};
	float projectileSpeed{};

	int frameCounter{};

	std::vector<std::vector<glm::vec2>> texturePosition;

	//stores effects that are influencing the enemy
	//gets added in the plant class, happens when collision happens
	std::vector<Effect> effects;

	std::vector<Effect> attackEffects;

	bool hitByPiercing{false};

	virtual bool attackFunc(glm::vec2 &playerPosition){
		std::cout << "illegal action performed, enemy tried attacking, not an enemy";
	};
	virtual void move(){
		std::cout << "illegal action performed, enemy tried moving, not an enemy";
	};
	virtual glm::vec2 getProjectileStartPositionForEnemy(Projectile &p){
		std::cout << "illegal action performed, tried to spawn enemy projectile, not an enemy";
	}
	void addAttackTimer(float dt){
		if(attackingTimer + dt >= 3 - attackSpeed){
			attacking = false;
			attackingTimer = 0;
			return;
		}else{
			attackingTimer += dt;
		}
	}
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
	void addEffects(Projectile &p){
		for(int i{}; i < p.effects.size(); ++i){
			this->effects.push_back(p.effects[i]);
		}
	}
};
#endif