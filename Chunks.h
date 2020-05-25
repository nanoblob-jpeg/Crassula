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
	std::vector<GameObject*> objects;
	std::string name;
	std::vector<bool> locationOfObjects;
	//plants locations are in 10x10 grid
	std::vector<Plant> plants;
	
	Chunk(){}
	void Load(const char *file);
	void Draw(SpriteRenderer &renderer);
private:
	void init(std::vector<std::string> files);
};

#endif