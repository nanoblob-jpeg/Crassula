#include "Chunks.h"

void Chunks::Load(const char *file){
	std::vector<std::string> files;

	std::string line;
	std::ifstream fstream(file);
	if(fstream){
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

void Chunks::init(std::vector<std::string> files){
	/*
	files are written as follows:
	posx
	posy
	width
	height
	color_r
	color_g
	color_b
	texture_name
	*/
	if(files.size() == 0){
		std::cout << "no files detected";
		return;
	}
	for(int i{}; i < files.size(); ++i){
		std::string line;
		std::ifstream fstream(files[i]);
		glm::vec2 pos, size;
		std::string textureName;
		glm::vec3 color;
		if(fstream){
			std::getline(fstream, line);
			pos[0] = std::stof(line);
			std::getline(fstream, line);
			pos[1] = std::stof(line);
			std::getline(fstream, line);
			size[0] = std::stof(line);
			std::getline(fstream, line);
			size[1] = std::stof(line);
			std::getline(fstream, line);
			color[0] = std::stof(line);
			std::getline(fstream, line);
			color[1] = std::stof(line);
			std::getline(fstream, line);
			color[2] = std::stof(line);
			std::getline(fstream, textureName);
			GameObject obj(pos, size, ResourceManager::GetTexture(textureName.c_str()), color);
			objects.push_back(obj);
		}else{
			std::cout << "could not open object file";
		}
	}
}

