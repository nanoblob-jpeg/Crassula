#include "Plant.h"
Plant::Plant(std::string pname, int plevel, int pattack, int prange, bool ppiercing, Texture tex, glm::vec2 psize, glm::vec3 pcolor)
	: name(pname), level(plevel), attack(pattack), range(prange), piercing(ppiercing)
	{
		interactable = true;
		sprite = tex;
		size = psize;
		color = pcolor;
	};

void Plant::addEffect(Effect effect){
	effects.push_back(effect);
}

void Plant::applyEffects(Enemy &e){
	for(int i{}; i < effects.size(); ++i){
		e.effects.push_back(effects[i]);
	}
}

void Plant::applyDamage(Enemy &e){
	e.health -= attack;
}