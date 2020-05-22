#ifndef CHUNKS
#define CHUNKS

#include <vector>
#include <string>
#include "GameObject.h"

class Chunk{
public:
	std::vector<GameObject*> objects;
	std::string name;
	std::vector<bool> locationOfObjects;
	Chunk(){}
	void Load(const char *file);
	void Draw(SpriteRenderer &renderer);
private:
	void init(std::vector<std::string> files);
};

#endif