#ifndef BACKGROUND
#define BACKGROUND
class ResourceManager;
#include "Texture.h"
#include <string>
#include "ResourceManager.h"
class Background{
public:
	std::string name;
	Texture layerOne;
	Texture layerTwo;
	Texture layerThree;

	Background(){}
	Background(std::string &name, std::string &one, std::string &two, std::string &three);
};
#endif