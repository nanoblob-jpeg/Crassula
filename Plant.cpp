#include "Game.h"
Plant::Plant(std::string pname, Texture &tex, glm::vec2 psize, glm::vec2 texCoord, glm::vec3 pcolor)
	: name(pname), level(0)
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

int Plant::interact(Game *game){
	//testing if the player already has a plant of the same type
	for(int i{}; i < game->player.numPlants; ++i){
		if(game->player.plants[i]->name.compare(this->name) == 0){
			if(game->player.plants[i]->level == 4)
				return -1;
			if(!(game->player.plants[i]->level == 3 && numCurrentLevelFourPlants == game->player.bowl->numOfLevelFour)){
				if(game->player.plants[i]->level == 3){
					game->player.plants[i]->level += 1;
					game->player.numCurrentLevelFourPlants += 1;
					return 0;
				}else{
					game->player.plants[i]->level += 1;
					return 0;
				}
			}
		}
	}
	if(game->player.numPlants == 0){
		game->player.plants[0] = this;
		this->level = 1;
		game->player.currentPlant = 0;
		game->player.numPlants = 1;
		return 0;
	}
	if(game->player.numPlants == game->player.bowl->numOfPlants){
		game->player.plants[currentPlant] = this;
		this->level = 1;
		return 0;
	}else{
		game->player.plants[numPlants] = this;
		this->level = 1;
		game->player.numPlants += 1;
		return 0;
	}
}