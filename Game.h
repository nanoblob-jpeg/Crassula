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
#include <limits>


#ifndef GAME_H
#define GAME_H

class Game
{
public:
	//CONSTANTS
	//max texture sizes
	const short maxPlantSize{50};
	const short maxProjectileSize{30};
	const short maxEnemyProjectileSize{30};
	const short maxEnemySize{40};
	const short maxPlantIconSize{20};
	const short maxAchievementSize{45};
	const float maxLevelIconSize{28.0};
	const float maxEffectIconSize{20.0};
	const float maxTextWidth{20};
	const float maxTextHeight{40};
	const short effectIconSpacing{5};
	const float backgroundSize{2500};

	//texture offset constants
	const short plantBoxSize{35};
	const short plantBoxSpacing{15};
	const short threePlantFirstBoxX{253};
	const short fourPlantFirstBoxX{203};
	const short plantFirstBoxY{-258};
	const short levelBarSpacing{3};

	//movement constants
	const float maxSpeed = 150.0;
	const float acceleration = 130.0;

	//game board size constants
	const short distanceFromCenterForGeneration{4500};
	const short regionSize{5000};
	const short chunkSize{500};
	const short blockSize{50};
	const short edgeDistace{7500};
	const short innerRegionEdgeDistance{2500};

	//num game object constants
	const unsigned short numOfChunks{40};
	const unsigned short numOfPlants{1};
	const unsigned short numOfEnemies{1};
	const int numAchievements{140};
	const int numGreenHouse{1};

	//armory vars
	int bowlCounter;
	bool armoryButtonPress = false;

	//screen size constants
	float Width, Height;

	//gamestates
	enum GameState{
		START_SCREEN,
		GAME_ACTIVE_CLASSIC,
		DEATH_SCREEN,
		HOME_ARMORY,
		HOME_ACHIEVEMENTS,
		HOME_GREENHOUSE,
		HOME_MAIN
	};

	//unlock stuff logic
	std::vector<bool> bowls{};
	std::vector<std::string> bowlNames{};

	//misc game drivers
	GameState m_state;
	bool Keys[1024]{};
	std::mt19937 mersenne{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };

	Camera cam = Camera(glm::vec3(0.0f, 0.0f, 1.0f));

	//achievement system
	std::vector<bool> completedAchievements{};
	//distance achievements
	float chunksTravelledThrough;
	float chunksFallenThrough{};
	glm::vec2 distanceTravelled{};
	bool outOfFirstChunk = false;

	//achievement room logic
	int achievementSelector{};
	bool achievementMoved = false;
	bool viewingAchievement = false;
	std::vector<std::string> achievementNames{};
	std::vector<glm::vec2> achievementOffsets{};
	std::vector<float> achievementTexCoords{};

	//greenhouse room logic
	//these icons are 55x55
	std::vector<bool> unlockedPlants{};
	int plantSelector{};
	bool greenhouseMoved = false;
	bool selectPressed = false;
	std::vector<glm::vec2> selectedPlantOffset{};
	//this also keeps track of what plants are selected
	std::vector<float> selectedPlantTexCoords{};
	std::vector<std::string> plantNames{};
	std::vector<std::string> boostNames{};
	std::vector<glm::vec2> greenhouseOffsets{};
	std::vector<float> greenhouseTexCoords{};
	std::vector<float> greenhouseLevels{};
	std::vector<float> greenhouseExperience{};
	std::vector<glm::vec2> greenhouseLevelOffset{};

	//Player object
	Player player;
	int playerHealth;
	//uses points for recoveryTimer, this var is jsut to keep track of what the last
	//value was
	float recoveryTimer{};
	int maxHealth;

	//movement variables:
	float upCounter{};

	//game storage system
	std::deque<std::deque<std::vector<Chunk>>> board{};
	std::vector<Enemy*> board_enemies{};
	std::vector<Projectile> enemy_projectiles{};
	std::vector<Projectile> player_projectiles{};
	float points{};
	float enemyMultiplier{1.0};

	//RENDER LOGIC VARIABLES
	bool generatedChunks{false};

	//block vars
	std::vector<glm::vec2> blockOffsets{};
	int numBlocks{};
	
	//plant vars
	std::vector<glm::vec2> plantOffsets{};
	std::vector<float> plantTexCoords{};
	int numPlants{};

	//enemy projectile vars
	std::vector<glm::vec2> enemyProjectileOffsets{};
	std::vector<float> enemyProjectileTexCoords{};

	//player projectile vars
	std::vector<glm::vec2> playerProjectileOffsets{};
	std::vector<float> playerProjectileTexCoords{};

	//enemy vars
	std::vector<glm::vec2> enemyOffsets{};
	std::vector<float> enemyTexCoords{};
	std::vector<float> hitData{};

	//background render logic stuff
	Background *backgroundTextures = nullptr;
	glm::vec2 backgroundLayerOneOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerTwoOffset{0.0f, 0.0f};
	glm::vec2 backgroundLayerThreeOffset{0.0f, 0.0f};

	//text render stuff
	std::vector<glm::vec2> textOffsets{};
	std::vector<float> textTexCoords{};
	std::vector<glm::vec2> pointOffsets{};
	std::vector<float> pointTexCoords{};
	glm::vec2 deathScreenCoords{};

	//ICON RENDER VARS
	//effect icon vars
	std::vector<glm::vec2> effectsIconOffsets{};
	std::vector<float> effectsIconTexCoords{};
	int numEffectIcon{};

	//plant icon vars
	std::vector<glm::vec2> plantIconOffsets{};
	std::vector<float> plantIconTexCoords{};
	int numPlantIcon{};

	//highlight var
	glm::vec2 highlightPosition{};

	//level icon vars
	std::vector<glm::vec2> levelIconOffsets{};
	std::vector<float> levelIconTexCoords{};

	//effect icon vars
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
	void checkAchievements();
	void setAchievementsToTrue(int start, int stop);
	void setUnlockedBowls();
	void prepAchievementScreen();
	void loadGameData();
	void prepGreenhouse();
	void greenhouseSelectorHelper(int avoid);

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
	void renderStartScreen();
	void renderHomeMain();
	void renderArmoryScreen();
	void renderAchievements();
	void renderDeathScreen();
	void renderGreenhouse();
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
	void calculateTextRenderValues();

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
	void initStaticImageRenderer(glm::mat4 &projection);
};
#endif