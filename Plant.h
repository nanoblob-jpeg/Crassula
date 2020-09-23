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
	std::string textureName;
	int level;

	std::vector<std::string> projectileName;
	Plant(){};
	Plant(std::string pname, std::string ptname, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f));

	int interact(Game *game);
	std::string getProjectileName();
};
#endif