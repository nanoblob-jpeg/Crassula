#include "Background.h"
Background::Background(std::string &pname, std::string &one, std::string &two, std::string &three){
	name = pname;
	layerOne = ResourceManager::GetTexture(one);
	layerTwo = ResourceManager::GetTexture(two);
	layerThree = ResourceManager::GetTexture(three);
}