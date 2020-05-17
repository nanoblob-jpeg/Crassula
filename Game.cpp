#include "Game.h"

SpriteRenderer *Renderer;

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

	//todo  load in the player object
	//todo  this is also going to need deserialization so maybe add it
	//todo  into the class so it does it by itself
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
	if(Keys[GLFW_KEY_W]){
		if(upCounter == 0)
			player.falling = true;
		//need to add code in the collision detector that will change falling to false
		if(upCounter < 10){
			upCounter++;
			Player.velocity.y = 3.5;
		}
	}
	if(Keys[GLFW_KEY_S]){
		//todo
		//make this call a switch weapon method in the Player class
		//for now it will do nothing
	}
	//move left, with correct acceleration
	if(Keys[GLFW_KEY_A]){
		player.velocity.x -= player.speed;
		if(player.velocity.x < 0)
			player.velocity.x = std::max(player.velocity.x, (-3.5) - (speed/4));
	}
	//move right, with correct acceleration
	if(Keys[GLFW_KEY_D]){
		player.velocity.x += player.speed;
		if(player.velocity.x > 0)
			player.velocity.x = std::min(player.velocity.x, 3.5 + (speed/4));
	}
	//slowing down, correct acceleration
	if(player.velocity.x > 0 && !player.velocity.x && !player.velocity.y){
		if(player.velocity.x < 0){
			if(player.velocity.x > -player.speed - 0.2;)
				player.velocity.x = 0;
			else
				player.velocity.x += player.speed + 0.2;
		}else{
			if(player.velocity.x < player.speed + 0.2)
				player.velocity.x = 0;
			else
				player.velocity.x -= (player.speed + 0.2);
		}
	}
	if(player.falling && counter > 1){
		player.velocity.y -= 0.6;
	}
	if(upCounter != 0 && !player.falling){
		upCounter = 0;
	}
	cam.ProcessKeyboard(player.velocity, dt);
};

