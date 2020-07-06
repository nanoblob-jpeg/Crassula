#include <vector>
#include "Camera.h"
#include "SpriteRenderer.h"
#include "Enemy.h"
#include "Melee.h"
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
	//constants
	short maxPlantSize{20};
	short maxProjectileSize{20};
	short maxEnemyProjectileSize{30};
	short maxEnemySize{40};
	short maxPlantIconSize{20};
	float maxLevelIconSize{28.0};
	float maxEffectIconSize{20.0};
	short effectIconSpacing{5};
	float backgroundSize{2500};

	short plantBoxSize{35};
	short plantBoxSpacing{15};
	short threePlantFirstBoxX{253};
	short fourPlantFirstBoxX{203};
	short plantFirstBoxY{-258};
	short levelBarSpacing{3};

	float maxSpeed = 150.0;
	float acceleration = 130.0;

	short distanceFromCenterForGeneration{4500};
	short regionSize{5000};
	short chunkSize{500};
	short blockSize{50};
	short edgeDistace{7500};
	short innerRegionEdgeDistance{2500};

	unsigned short numOfChunks{40};
	unsigned short numOfPlants{4};
	unsigned short numOfEnemies{1};

	float Width, Height;

	//gamestates
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
	bool Keys[1024]{};
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

	Camera cam = Camera(glm::vec3(0.0f, 0.0f, 1.0f));

	//Player object
	Player player;
	//movement variables:
	float upCounter{};
	//game storage system
	std::deque<std::deque<std::vector<Chunk>>> board{};
	std::vector<Enemy*> board_enemies{};
	std::vector<Projectile> enemy_projectiles{};
	std::vector<Projectile> player_projectiles{};
	float points{};

	//render logic stuff
	bool generatedChunks{false};

	std::vector<glm::vec2> blockOffsets{};
	int numBlocks{};
	
	std::vector<glm::vec2> plantOffsets{};
	std::vector<float> plantTexCoords{};
	int numPlants{};

	std::vector<glm::vec2> enemyProjectileOffsets{};
	std::vector<float> enemyProjectileTexCoords{};

	std::vector<glm::vec2> playerProjectileOffsets{};
	std::vector<float> playerProjectileTexCoords{};

	std::vector<glm::vec2> enemyOffsets{};
	std::vector<float> enemyTexCoords{};

	//background render logic stuff
	Background *backgroundTextures = nullptr;
	glm::vec2 backgroundLayerOneOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerTwoOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerThreeOffset{0.0f, 0.0f};

	//text render stuff
	std::vector<GameObject *> text{};

	//icon render stuff
	std::vector<glm::vec2> effectsIconOffsets{};
	std::vector<float> effectsIconTexCoords{};
	int numEffectIcon{};

	std::vector<glm::vec2> plantIconOffsets{};
	std::vector<float> plantIconTexCoords{};
	int numPlantIcon{};

	glm::vec2 highlightPosition{};

	std::vector<glm::vec2> levelIconOffsets{};
	std::vector<float> levelIconTexCoords{};

	std::vector<glm::vec2> effectIconOffsets{};
	std::vector<float> effectIconTexCoords{};

	Game(unsigned int width, unsigned int height);
	~Game();
	//loads all textures
	void Init();
	//game loop
	void ProcessInput(float dt);
	void Update(float dt);
	void Render();

	//game initialization/destruction
	void initializeGame();
	void loadEnemies();
	void clearAndResetGameBoard();
	void setBackground(std::string &name);
	void prepBoard();
	void reserveArraySpace();
	void gameEndProtocol();

	//Chunk Generation
	void prepBoardForChunkCreation(const short direction);
	void generateChunks(const short direction);
	void despawnEnemiesFromDeletedChunks(const short direction);
	void despawnProjectilesFromDeletedChunks(const short direction);
	void fixGeneratedEnemiesPosition(const short i, const short j, const short k, const short direction);
	void fixRemainingEnemyPosition(const short direction);
	void fixRemainingProjectilePosition(const short direction);
	short findAddingAmountOffsetWhenGeneratingChunks(const short direction);
	void fixPlayerPosition(const short direction);
	void spawnPlant(std::vector<Chunk> &temp, short i, short j);
	void spawnEnemy(short i, short j, short k, short l);

	//collision detection
	void player_and_object_collisions(GameObject *object, const float dt, const short gameobject_offset_x = 0, const short gameobject_offset_y = 0);
	void player_and_object_collisions(GameObject *object, const float dt, const short index_x, const short index_y, const short index_chunk, const short gameobject_offset_x = 0, const short gameobject_offset_y = 0);
	bool game_classic_two_object_collisions(GameObject *object, GameObject *projectile);
	bool game_classic_two_object_collisions(GameObject *object, GameObject *object2, const short width_offset, const short height_offset);
	bool game_classic_two_object_collisions(glm::vec2 position, glm::vec2 size, GameObject *projectile);
	void findLocationCoordinates(short &width, short &height, const float x, const float y);
	void nineBlockCollisionDetectionPAndO(const short width, const short height, const float dt);
	bool nineBlockCollisionDetectionGeneral(const short width, const short height, GameObject *object);
	void player_projectile_collision_detection();
	void enemy_projectile_collision_detection();
	short findPlayerDirection(GameObject *object, const float dt, const short gameobject_offset_x = 0, const short gameobject_offset_y = 0);
	glm::vec4 findInteractPosition(GameObject *object, short index_x, short index_y, short index_chunk);

	//game logic
	void spawnPlayerProjectile();
	glm::vec2 getProjectileStartPositionForPlayer(Projectile &p);
	void clearDeadEnemies();
	void processEffectsForEnemies(const float dt);
	void processEffectsForPlayer(const float dt);
	void processPlayerMovement(const float dt);
	void moveAllProjectiles(const float dt);
	void processPlantInteraction();
	void enemyAttackLogic();

	//rendering
	void renderHomeMain();
	void renderGame();
	void renderGameBackground(glm::mat4 &view);
	void renderBlocks(glm::mat4 &view);
	void renderPlants(glm::mat4 &view);
	void renderEnemyProjectiles(glm::mat4 &view);
	void renderPlayerProjectiles(glm::mat4 &view);
	void renderEnemies(glm::mat4 &view);
	void renderPlayer(glm::mat4 &view);
	void renderText(glm::mat4 &view);
	void renderUI(glm::mat4 &view);

	//calculating render values
	void calculateNewRenderValues();
	void calculateBlockOffsets(const short i, const short j);
	void calculatePlantOffsets(const short i, const short j);
	void calculateProjectileRenderValues();
	void calculateEnemyRenderValues();
	void calculateIconRenderValues();
	void moveBackground(float dt);
	void findHighlightPosition();
	void findLevelIconPosition();

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
	void initUIRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initIconRenderer(glm::mat4 &view, glm::mat4 &projection);
	void initEnemyProjectileRenderer(glm::mat4 &view, glm::mat4 &projection);
};
#endif