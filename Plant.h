#include <string>
#include <vector>
#include "Effect.h"
#include "Enemy.h"
#include <glm/glm.hpp>
#include "Texture.h"
#include "GameObject.h"

#ifndef PLANT
#define PLANT
class Plant : public GameObject{
public:
	//position for the game is in the 10x10 grid
	std::string name;
	int level;
	int attack;
	int range;
	std::vector<Effect> effects;
	bool piercing;

	Plant(std::string pname, int plevel, int pattack, int prange, bool ppiercing, Texture tex, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f));
	void addEffect(Effect effect);
	void applyEffects(Enemy &e);
	void applyDamage(Enemy &e);
};
#endif