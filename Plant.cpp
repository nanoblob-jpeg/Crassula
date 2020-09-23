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
	//setting the greenhouse plant as unlocked
	auto it = std::find_if(game->greenhouse.begin(), game->greenhouse.end(), [=](greenHousePlant p){return p.plantName == name;});
	if(it != game->greenhouse.end())
		if(!game->unlockedPlants[std::distance(game->greenhouse.begin(), it)]){
			game->unlockedPlants[std::distance(game->greenhouse.begin(), it)] = true;
		}
	//testing if the player already has a plant of the same type
	for(int i{}; i < game->player.getNumPlants(); ++i){
		Plant* temp = game->player.getPlant(i);
		if(temp->name.compare(this->name) == 0){
			if(temp->level == 4)
				return -1;
			if(!(temp->level == 3 && game->player.getNumCurrentLevelFourPlants() == game->player.getMaxNumOfLevelFour())){
				if(temp->level == 3){
					temp->level += 1;
					game->player.addToNumCurrentLevelFourPlants(1);
					return 0;
				}else{
					temp->level += 1;
					return 0;
				}
			}
		}
	}
	if(game->player.numPlants == 0){
		game->player.addPlant(this->name, true);
		Plant* temp = game->player.getPlant(0);
		temp->level = 1;
		game->player.setCurrentPlant(0);
		game->player.addNumPlants(1);
		return 0;
	}
	if(game->player.getNumPlants() == game->player.getMaxNumPlants()){
		game->player.addPlant(this->name, false);
		Plant* temp = game->player.getCurrentPlant();
		temp->level = 1;
		return 0;
	}else{
		game->player.addPlant(this->name, true);
		Plant* temp = game->player.getPlant(game->player.getNumPlants());
		temp->level = 1;
		game->player.addNumPlants(1);
		return 0;
	}
}

std::string Plant::getProjectileName(){
	return projectileName[level - 1];
}