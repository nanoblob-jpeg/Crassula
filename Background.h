#ifndef BACKGROUND
#define BACKGROUND
#include "Texture.h"
#include <string>
#include "ResourceManager.h"
class Background{
public:
	Texture layerOne;
	Texture layerTwo;
	Texture layerThree;

	Background(std::string one, std::string two, std::string three){
		layerOne = ResourceManager::GetTexture(one);
		layerTwo = ResourceManager::GetTexture(two);
		layerThree = ResourceManager::GetTexture(three);
	}
}
#endif