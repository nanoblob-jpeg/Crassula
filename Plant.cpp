#include "Plant.h"
Plant::Plant(std::string pname, int plevel, int pattack, int prange, bool ppiercing, Texture &tex, glm::vec2 psize, glm::vec2 texCoord, glm::vec3 pcolor)
	: name(pname), level(plevel), attack(pattack), range(prange), piercing(ppiercing)
	{
		interactable = true;
		sprite = tex;
		size = psize;
		color = pcolor;
		int temp = std::max(psize[0], psize[1]);
		unsigned int texWidth = sprite.m_width;
		unsigned int texHeight = sprite.m_height;
		
		glm::vec2 tempCoord;
		tempCoord[0] = (texCoord[0]/texWidth);
		tempCoord[1] = ((texCoord[1] + size[1])/texHeight);
		texturePosition.push_back(tempCoord);

		tempCoord[0] = ((texCoord[0] + size[0])/texWidth);
		tempCoord[1] = (texCoord[1]/texHeight);
		texturePosition.push_back(tempCoord);


		tempCoord[0] = (texCoord[0]/texWidth);
		tempCoord[1] = (texCoord[1]/texHeight);
		texturePosition.push_back(tempCoord);

		texturePosition.push_back(texturePosition[0]);

		tempCoord[0] = ((texCoord[0] + size[0])/texWidth);
		tempCoord[1] = ((texCoord[1] + size[1])/texHeight);
		texturePosition.push_back(tempCoord);

		texturePosition.push_back(texturePosition[1]);
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