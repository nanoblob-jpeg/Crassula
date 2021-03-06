#ifndef CHUNKS
#define CHUNKS

#include <vector>
#include <string>
#include "GameObject.h"
#include "Plant.h"
#include "Enemy.h"

class Chunk{
public:
	//these objects are stored in normal pixel coordinates, not 10x10 grid
	//these are the blocks
	std::vector<GameObject*> objects;
	
	std::string name;
	std::vector<bool> locationOfObjects;
	//plants locations are in normal pixel coordinates for easier collision detection
	std::vector<Plant> plants;
	
	Chunk(){}
	void Load(const char *file);
	void removePlant(float in);
private:
	void init(std::vector<std::string> &files);
};
#endif