#ifndef CHUNKS
#define CHUNKS

#include <vector>
#include <string>
#include "GameObject.h"
#include "ResourceManager.h"

class Chunks{
public:
	std::vector<GameObject> objects;
	Chunks(){}
	void Load(const char *file);
	void Draw(SpriteRenderer &renderer);
private:
	void init(std::vector<std::string> files);
};

#endif