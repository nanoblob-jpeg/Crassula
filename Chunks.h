#ifndef CHUNKS
#define CHUNKS

#include <vector>
#include <string>
#include "GameObject.h"
#include "Plant.h"
#include "Enemy.h"

class Chunk{
public:
	std::vector<GameObject*> objects;
	std::string name;
	std::vector<bool> locationOfObjects;
	std::vector<Plant> plants;
	
	Chunk(){}
	void Load(const char *file);
	void Draw(SpriteRenderer &renderer);
private:
	void init(std::vector<std::string> files);
};

#endif