#include "Game.h"
Plant::Plant(std::string pname, std::string ptname, glm::vec2 psize, glm::vec3 pcolor)
	: name(pname), level(0)
{
	interactable = true;
	size = psize;
	color = pcolor;
	position = glm::vec2((50-size[0])/2, (size[1] - 50));
	textureName = ptname;
	type = "plant";
};

int Plant::interact(Game *game){
	//testing if the player already has a plant of the same type
	for(int i{}; i < game->player.numPlants; ++i){
		if(game->player.plants[i].name.compare(this->name) == 0){
			if(game->player.plants[i].level == 4)
				return -1;
			if(!(game->player.plants[i].level == 3 && game->player.numCurrentLevelFourPlants == game->player.bowl->numOfLevelFour)){
				if(game->player.plants[i].level == 3){
					game->player.plants[i].level += 1;
					game->player.numCurrentLevelFourPlants += 1;
					return 0;
				}else{
					game->player.plants[i].level += 1;
					return 0;
				}
			}
		}
	}
	if(game->player.numPlants == 0){
		game->player.plants.push_back(ResourceManager::GetPlant(this->name));
		game->player.plants[0].level = 1;
		game->player.currentPlant = 0;
		game->player.numPlants = 1;
		return 0;
	}
	if(game->player.numPlants == game->player.bowl->numOfPlants){
		game->player.plants[game->player.currentPlant] = ResourceManager::GetPlant(this->name);
		game->player.plants[game->player.currentPlant].level = 1;
		return 0;
	}else{
		game->player.plants.push_back(ResourceManager::GetPlant(this->name));
		game->player.plants[game->player.numPlants].level = 1;
		game->player.numPlants += 1;
		return 0;
	}
}