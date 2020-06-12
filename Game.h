#include <vector>
class GameObject;
#include "Camera.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Chunks.h"
#include "Plant.h"
#include "Bowl.h"
#include "Player.h"
#include "Effect.h"
#include "Projectile.h"
#include "TexSampRenderer.h"
#include "BackgroundRenderer.h"
#include "Background.h"

#include "Melee.h"

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
	const int maxPlantSize{20};
	const int maxProjectileSize{20};
	const int maxEnemySize{40};

	enum GameState{
		START_SCREEN,
		GAME_ACTIVE_CLASSIC,
		GAME_ACTIVE_BOSS,
		DEATH_SCREEN,
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
	unsigned int numOfPlants{4};
	unsigned int numOfEnemies{1};
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

	Camera cam = Camera(glm::vec3(0.0f, 0.0f, 1.0f));

	//Player object
	Player player;
	//movement variables:
	float upCounter{};
	
	//game storage system
	std::deque<std::deque<std::vector<Chunk>>> board;
	std::vector<Enemy> board_enemies;
	std::vector<Projectile> enemy_projectiles;
	std::vector<Projectile> player_projectiles;
	float points{};

	//render logic stuff
	std::vector<glm::vec2> blockOffsets{};
	int numBlocks{};
	bool generatedChunks{false};
	std::vector<glm::vec2> plantOffsets{};
	std::vector<glm::vec2> plantTexCoords{};
	int numPlants{};
	std::vector<glm::vec2> enemyProjectileOffsets{};
	std::vector<glm::vec2> enemyProjectileTexCoords{};
	std::vector<glm::vec2> playerProjectileOffsets{};
	std::vector<glm::vec2> playerProjectileTexCoords{};
	std::vector<glm::vec2> enemyOffsets{};
	std::vector<glm::vec2> enemyTexCoords{};
	//background render logic stuff
	Background *backgroundTextures;
	glm::vec2 backgroundLayerOneOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerTwoOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerThreeOffset{0.0f, 0.0f};
	//text render stuff
	std::vector<GameObject *> text;

	Game(unsigned int width, unsigned int height);
	~Game();
	//loads all textures
	void Init();
	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();
private:
	//game initialization/destruction
	void initializeGame();
	void loadEnemies();
	void clearAndResetGameBoard();
	void setBackground(std::string name);
	void prepBoard();
	void reserveArraySpace();
	void gameEndProtocol();

	//Chunk Generation
	void prepBoardForChunkCreation(int direction);
	void generateChunks(int direction);
	void despawnEnemiesFromDeletedChunks(int direction);
	void despawnProjectilesFromDeletedChunks(int direction);
	void fixGeneratedEnemiesPosition(int i, int j, int k, int direction);
	void fixRemainingEnemyPosition(int direction);
	void fixRemainingProjectilePosition(int direction);
	int findAddingAmountOffsetWhenGeneratingChunks(int direction);

	//collision detection
	void player_and_object_collisions(GameObject *object, float dt, int gameobject_offset_x = 0, int gameobject_offset_y = 0);
	bool game_classic_two_object_collisions(GameObject *object, GameObject *projectile);
	bool game_classic_two_object_collisions(GameObject *object, GameObject *object2, int width_offset, int height_offset);
	void findLocationCoordinates(int &width, int &height, float x, float y);
	void nineBlockCollisionDetectionPAndO(int width, int height, float dt);
	bool nineBlockCollisionDetectionGeneral(int width, int height, GameObject *object);
	void player_projectile_collision_detection();
	void enemy_projectile_collision_detection();
	short findPlayerDirection(GameObject *object, float dt, int gameobject_offset_x = 0, int gameobject_offset_y = 0);
	
	//game logic
	void spawnPlayerProjectile();
	glm::vec2 getProjectileStartPositionForPlayer(Projectile &p);
	void clearDeadEnemies();
	void processEffectsForEnemies(float dt);
	void processEffectsForPlayer(float dt);
	void processPlayerMovement(float dt);
	void moveAllProjectiles(float dt);

	//rendering
	void renderHomeMain();
	void renderGame();
	void renderGameBackground();
	void renderBlocks(glm::mat4 &view);
	void renderPlants(glm::mat4 &view);
	void renderEnemyProjectiles(glm::mat4 &view);
	void renderPlayerProjectiles(glm::mat4 &view);
	void renderEnemies(glm::mat4 &view);
	void renderPlayer(glm::mat4 &view);
	void renderText();
	void calculateNewRenderValues();
	void calculateBlockOffsets(int i, int j);
	void calculatePlantOffsets(int i, int j);
	void calculateProjectileRenderValues();
	void calculateEnemyRenderValues();

	//Shader && Renderer loading
	void initShaders();
	void initRenderers();
	void initRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initBlockRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initPlantRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initProjectileRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initEnemyRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initBackgroundRenderers(glm::mat4 &projection);
	void initTextRenderer(glm::mat4 &view, glm::mat4 &projection);
};
#endif