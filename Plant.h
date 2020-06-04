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
	
	std::vector<glm::vec2> texturePosition;

	std::vector<std::string> projectileName;

	Plant(std::string pname, int plevel, Texture &tex, glm::vec2 size, glm::vec2 texCoord, glm::vec3 color = glm::vec3(1.0f));
};
#endif