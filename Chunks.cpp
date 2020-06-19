#include "Chunks.h"
#include "ResourceManager.h"

void Chunk::Load(const char *file){
	std::vector<std::string> files;
	/*
	chunks written as:
	name
	list of gameobjects that are part of the chunk
	*/
	std::string line;
	std::ifstream fstream(file);
	if(fstream){
		std::getline(fstream, line);
		name = line;
		while(std::getline(fstream, line)){
			files.push_back(line);
		}
	}else{
		std::cout << "File not opened";
	}
	if(files.size() > 0)
		this->init(files);
	else
		std::cout << "Parsed zero lines";
}

void Chunk::init(std::vector<std::string> &names){
	if(names.size() == 0){
		std::cout << "no files detected";
		return;
	}
	for(int i{}; i < names.size(); ++i){
		objects.push_back(&ResourceManager::GetGameObject(names[i]));
	}
	for(int i{}; i < 100; ++i){
		locationOfObjects.push_back(false);
	}
	for(int i{}; i < objects.size(); ++i){
		locationOfObjects[(objects[i]->position.y/50) * -10 + (objects[i]->position.x/50)] = true;
	}
}

void Chunk::removePlant(float in){
	plants.erase(plants.begin() + in);
}