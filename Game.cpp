#include "Game.h"
SpriteRenderer *Renderer;
#include <glm/gtx/string_cast.hpp>
void Game::Init(){
	//load shaders
	ResourceManager::LoadShader("shaders/vertexShader.txt", "shaders/fragShader.txt", nullptr, "sprite");
	//configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
	glm::mat4 view = cam.GetViewMatrix();
	Shader sProgram;
	sProgram = ResourceManager::GetShader("sprite");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
	/*
	load textures
	 */
	
	ResourceManager::LoadTexture("awesomeface.png", true, "face");

	/*
	load chunks
	 */
	//loading home_main chunk
	home_main.Load("home_main.txt");
	//parsing list of chunks ofr the 10x10 squares
	std::vector<std::string> chunk_list;

	std::string line;
	std::ifstream fstream("chunk_list.txt");
	if(fstream){
		while(std::getline(fstream, line)){
			chunk_list.push_back(line);
		}
	}else{
		std::cout << "File not opened";
	}
	//loading 10x10 squares
	for(int i{}; i < chunk_list.size(); ++i){
		Chunks temp;
		temp.Load(chunk_list[i].c_str());
		chunks.push_back(temp);
	}
}

Game::~Game(){
	delete Renderer;
}

void Game::Render(){
	Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200.0f, 200.0f), glm::vec2(50.0f, 50.0f),
			45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	
}

void Game::Update(float dt){};

void Game::ProcessInput(float dt){

};

