#include <vector>

#include "Camera.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Chunks.h"
#include "Plant.h"
#include "Bowl.h"
#include "Player.h"
#include "Effect.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <deque>
#include <random>
#include <ctime>

#ifndef GAME_H
#define GAME_H

class Game
{
public:
	enum GameState{
		START_SCREEN,
		GAME_ACTIVE_CLASSIC,
		GAME_ACTIVE_BOSS,
		HOME_ARMORY,
		HOME_NURSERY,
		HOME_GREENHOUSE,
		HOME_CLOCK,
		HOME_MAIN
	};

	GameState m_state;
	bool Keys[1024];
	unsigned int Width, Height;
	unsigned int numOfChunks{40};
	unsigned int numOfPlants{10};
	unsigned int numOfEnemies{10};
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

	Camera cam = Camera(glm::vec3(0.0f, 0.0f, 1.0f));

	//Player object
	Player player;
	//movement variables:
	float upCounter{};
	
	//game storage system
	std::deque<std::deque<std::vector<Chunk>>> board;

	Game(unsigned int width, unsigned int height):m_state(START_SCREEN), Keys(), Width(width), Height(height){};
	~Game();
	//loads all textures
	void Init();
	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
private:
	void generateChunk(int x, int y);
	void loadEnemies();
};
#endif