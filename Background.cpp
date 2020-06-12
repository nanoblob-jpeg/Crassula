#include "Background.h"
Background::Background(std::string one, std::string two, std::string three){
	layerOne = ResourceManager::GetTexture(one);
	layerTwo = ResourceManager::GetTexture(two);
	layerThree = ResourceManager::GetTexture(three);
}