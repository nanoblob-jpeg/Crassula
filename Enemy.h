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

	//stores effects that are influencing the enemy
	//gets added in the plant class, happens when collision happens
	std::vector<Effect> effects;

	virtual void attack();
	virtual void move();
	void applyEffects(){
		for(int i{}; i < effects.size(); ++i){
			effects[i].applyEffects(health, defense, attack, speed, 0);
		}
	};
};
#endif