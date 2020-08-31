#include "Game.h"
#include "GameObject.h"

SpriteRenderer *Renderer;
SpriteRenderer *BlockRenderer;
SpriteRenderer *UIRenderer;
SpriteRenderer *staticImageRenderer;
//background renderer
BackgroundRenderer *BackGround_l1;
BackgroundRenderer *BackGround_l2;
BackgroundRenderer *BackGround_l3;
//all block, plant, and projectile textures need to have the
//same space or else I can't load the model matrix
TexSampRenderer *PlantRenderer;
TexSampRenderer *ProjectileRenderer;
TexSampRenderer *EnemyProjectileRenderer;
TexSampRenderer *EnemyRenderer;
TexSampRenderer *IconRenderer;
TexSampRenderer *textRenderer;

Game::Game(unsigned int width, unsigned int height){
	m_state = START_SCREEN;
	Width = width;
	Height = height;
}

/**
Destructor for the Game class
- deallocates memory used for the different renderers
- writes game data onto corresponding directory files
*/
Game::~Game(){
	delete Renderer;
	delete BlockRenderer;
	delete UIRenderer;
	delete staticImageRenderer;
	delete textRenderer;
	delete BackGround_l1;
	delete BackGround_l2;
	delete BackGround_l3;
	delete PlantRenderer;
	delete ProjectileRenderer;
	delete EnemyRenderer;
	delete EnemyProjectileRenderer;
	delete IconRenderer;

	std::ofstream file;
	file.open("bin/directories/player.txt");
	file << player.name << '\n';
	file << std::to_string(player.level) << '\n';
	file << backgroundTextures->name << '\n';
	file << std::to_string(player.experience) << '\n';
	file << player.bowl->name;
	file.close();

	file.open("bin/directories/gameData.txt");
	file << std::to_string(chunksTravelledThrough) << '\n';
	for(int i{}; i < selectedPlantTexCoords.size(); ++i){
		file << selectedPlantTexCoords[i] << '\n';
	}
	file.close();

	file.open("bin/directories/achievementUnlockFile.txt");
	for(int i{}; i < completedAchievements.size(); ++i){
		if(i != completedAchievements.size() - 1){
			if(completedAchievements[i])
				file << "1\n" << achievementNames[i] << '\n';
			else
				file << "0\n" << achievementNames[i] << '\n';
		}else{
			if(completedAchievements[i])
				file << "1\n" << achievementNames[i];
			else
				file << "0\n" << achievementNames[i];
		}
	}
	file.close();

	file.open("bin/directories/bowlUnlockFile.txt");
	for(int i{}; i < bowls.size(); ++i){
		if(i != bowls.size() - 1){
			if(bowls[i])
				file << "1\n" << bowlNames[i] << '\n';
			else
				file << "0\n" << bowlNames[i] << '\n';
		}else{
			if(bowls[i])
				file << "1\n" << bowlNames[i];
			else
				file << "0\n" << bowlNames[i];
		}
	}
	file.close();

	file.open("bin/directories/greenhouseUnlockFile.txt");
	for(int i{}; i < greenhouse.size(); ++i){
		if(unlockedPlants[i])
			file << "1\n";
		else
			file << "0\n";
		file << greenhouse[i].plantName << '\n';
		file << greenhouse[i].boostName << '\n';
		for(int j{}; j < greenhouse[0].boost.size(); ++j){
			file << greenhouse[i].boost[j] << '\n';
		}
		file << std::to_string(greenhouse[i].level) << '\n';
		file << std::to_string(greenhouse[i].experience);
		if(i != greenhouse.size() - 1)
			file << '\n';
	}
}

/**
loads the game object
- parses all of the different game files to load object, players, enemies, rooms
*/
void Game::Init(){
	initShaders();
	initRenderers();
	setUnlockedBowls();
	//load textures
	ResourceManager::LoadTexture2("bin/directories/textureDirectory.txt");
	ResourceManager::LoadArrayTextures("bin/directories/arrayTextureDirectory.txt");
	//load gameobject
	ResourceManager::LoadGameObject("bin/directories/gameObjectDirectory.txt");
	//loading chunks for the 10x10 squares
	ResourceManager::LoadChunk("bin/directories/chunk_list.txt", true);
	//load effects
	ResourceManager::LoadEffect("bin/directories/effect_list.txt");
	//load bowls
	ResourceManager::LoadBowl("bin/directories/bowl_list.txt");
	//load plants
	ResourceManager::LoadPlant("bin/directories/plant_list.txt");
	//loads backgrounds
	ResourceManager::LoadBackgrounds("bin/directories/background_list.txt");
	//load player
	player.loadPlayer("bin/directories/player.txt");
	//load projectiles
	ResourceManager::LoadProjectiles("bin/directories/projectileDirectory.txt");
	//load enemies
	loadEnemies();
	setBackground(player.backgroundName);
	//create board
	prepBoard();
	bowlCounter = ResourceManager::GetBowl(player.bowl->name).second;
	prepAchievementScreen();
	loadGameData();
	prepGreenhouse();
}

/**
handles all of the user input detection
@param: time elapsed since last frame
*/
void Game::ProcessInput(float dt){
	//this b key section is just for debugging and displaying when I need to
	if(Keys[GLFW_KEY_B]){
		for(int i{}; i < playedBowls.size(); ++i){
			std::cout << playedBowls[i] << ' ';
		}
		std::cout << '\n';
	}
	if(m_state == GAME_ACTIVE_CLASSIC){
		processPlayerMovement(dt);
		if(m_state == GAME_ACTIVE_CLASSIC){
			if(Keys[GLFW_KEY_SPACE]){
				if(player.canAttack(dt)){
					spawnPlayerProjectile();
					player.bowl->startAnimationCounter();
					player.bowl->addFrameTimer(dt);
				}else if(player.bowl->inAnimation){
					player.bowl->addFrameTimer(dt);
				}
			}
			if(Keys[GLFW_KEY_I]){
				if(player.interact){
					player.interact->interact(this);
					if(player.interact->type.compare("plant") == 0){
						processPlantInteraction();
					}
					enemyMultiplier += 0.2 + player.level/80.0;
					points += 5;
					player.experience += 2;
					plantsCollected++;
				}
			}
			if(Keys[GLFW_KEY_U]){
				if(!player.switchingPlants){
					player.switchPlant(false);
					player.switchingPlants = true;
				}
			}
			if(Keys[GLFW_KEY_O]){
				if(!player.switchingPlants){
					player.switchPlant(true);
					player.switchingPlants = true;
				}
			}
			if(player.switchingPlants && !Keys[GLFW_KEY_U] && !Keys[GLFW_KEY_O]){
				player.switchingPlants = false;
			}
		}
	}else if(m_state == START_SCREEN){
		if(Keys[GLFW_KEY_SPACE]){
			m_state = HOME_MAIN;
		}
	}else if(m_state == HOME_ARMORY){
		if(Keys[GLFW_KEY_A] && !armoryButtonPress){
			if(bowlCounter == 0){
				bowlCounter = ResourceManager::Bowls.size() - 1;
			}else{
				--bowlCounter;
			}
			armoryButtonPress = true;
		}
		if(Keys[GLFW_KEY_D]){
			if(bowlCounter == ResourceManager::Bowls.size() - 1){
				bowlCounter = 0;
			}else{
				++bowlCounter;
			}
			armoryButtonPress = true;
		}
		if(!Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D]){
			armoryButtonPress = false;
		}
		if(Keys[GLFW_KEY_I]){
			if(bowls[bowlCounter]){
				auto it = ResourceManager::Bowls.begin();
				while(bowlCounter){
					++it;
				}
				player.bowl = &(*it).second.first;
				bowlCounter = (*it).second.second;
			}
		}
		if(Keys[GLFW_KEY_Q]){
			m_state = HOME_MAIN;
			bowlCounter = ResourceManager::GetBowl(player.bowl->name).second;
		}	
	}else if(m_state == HOME_MAIN){
		if(Keys[GLFW_KEY_I]){
			gameStartProtocol();
		}
		if(Keys[GLFW_KEY_U]){
			m_state = HOME_ARMORY;
		}
		if(Keys[GLFW_KEY_O]){
			m_state = HOME_GREENHOUSE;
		}
		if(Keys[GLFW_KEY_J]){
			m_state = HOME_ACHIEVEMENTS;
			achievementSelector = 0;
		}
		if(Keys[GLFW_KEY_L]){
			m_state = START_SCREEN;
		}
		if(Keys[GLFW_KEY_K]){
			lookedAtCredits = true;
		}
	}else if(m_state == HOME_ACHIEVEMENTS){
		if(Keys[GLFW_KEY_W] && !achievementMoved){
			if(achievementSelector < 7){
				achievementSelector += 42;
			}else{
				achievementSelector -= 7;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_A] && !achievementMoved){
			if(achievementSelector%7 == 0){
				achievementSelector += 6;
			}else{
				achievementSelector -= 1;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_D] && !achievementMoved){
			if((achievementSelector-6)%7 == 0){
				achievementSelector -= 6;
			}else{
				achievementSelector += 1;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_S] && !achievementMoved){
			if(achievementSelector >= 42){
				achievementSelector -= 42;
			}else{
				achievementSelector += 7;
			}
			achievementMoved = true;
		}
		if(!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_D]){
			achievementMoved = false;
		}
		if(Keys[GLFW_KEY_I]){
			viewingAchievement = !viewingAchievement;
		}
		if(Keys[GLFW_KEY_Q]){
			m_state = HOME_MAIN;
		}
	}else if(m_state == DEATH_SCREEN){
		if(Keys[GLFW_KEY_Q]){
			m_state = HOME_MAIN;
		}
	}else if(m_state == HOME_GREENHOUSE){
		if(Keys[GLFW_KEY_W] && !greenhouseMoved){
			if(plantSelector < 10){
				plantSelector += 40;
			}else{
				plantSelector -= 10;
			}
			greenhouseMoved = true;
		}
		if(Keys[GLFW_KEY_A] && !greenhouseMoved){
			if(plantSelector%10 == 0){
				plantSelector += 9;
			}else{
				plantSelector -= 1;
			}
			greenhouseMoved = true;
		}
		if(Keys[GLFW_KEY_D] && !greenhouseMoved){
			if((plantSelector-9)%10 == 0){
				plantSelector -= 9;
			}else{
				plantSelector += 1;
			}
			greenhouseMoved = true;
		}
		if(Keys[GLFW_KEY_S] && !greenhouseMoved){
			if(plantSelector >= 40){
				plantSelector -= 40;
			}else{
				plantSelector += 10;
			}
			greenhouseMoved = true;
		}
		if(!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_D]){
			greenhouseMoved = false;
		}
		if(Keys[GLFW_KEY_1]){
			greenhouseSelectorHelper(0);
		}
		if(Keys[GLFW_KEY_2]){
			greenhouseSelectorHelper(1);
		}
		if(Keys[GLFW_KEY_3]){
			greenhouseSelectorHelper(2);
		}
		if(Keys[GLFW_KEY_4]){
			greenhouseSelectorHelper(3);
		}
		if(Keys[GLFW_KEY_5]){
			greenhouseSelectorHelper(4);
		}
		if(Keys[GLFW_KEY_6]){
			greenhouseSelectorHelper(5);
		}
		if(Keys[GLFW_KEY_Q]){
			m_state = HOME_MAIN;
		}
	}
};

/**
handles all of the calls for game logic and flow of the game
@param: time elapsed since last frame
*/
void Game::Update(float dt){
	timeAlive += dt;
	if(glm::vec2(cam.Position.x, cam.Position.y) == previousPosition){
		timeStill += dt;
		if(timeStill >= 60)
			time_60_still_passed = true;
	}else{
		previousPosition = glm::vec2(cam.Position.x, cam.Position.y);
	}

	//move the player
	cam.ProcessKeyboard(player.velocity, dt);
	player.interact = nullptr;
	if(player.bowl->inAnimation){
		player.bowl->addFrameTimer(dt);
	}
	
	//add to achievement data
	if(!outOfFirstChunk){
		if(abs(cam.Position[0]) > 250){
			outOfFirstChunk = true;
			if(!(chunksTravelledThrough == std::numeric_limits<float>::max())){
				chunksTravelledThrough++;
				gameChunksTravelledTrough++;
			}
			distanceTravelled.x = cam.Position[0];
		}else if(abs(cam.Position[1]) > 250){
			outOfFirstChunk = true;
			if(!(chunksTravelledThrough == std::numeric_limits<float>::max())){
				chunksTravelledThrough++;
				gameChunksTravelledTrough++;
			}
			if(cam.Position[1] < 0)
				if(!(chunksFallenThrough == std::numeric_limits<int>::max())){
					chunksFallenThrough++;
					gameChunksTravelledTrough++;
				}
			distanceTravelled.y = cam.Position[1];
		}
	}else{
		if(abs(distanceTravelled.x - cam.Position[0]) > 500){
			if(!(chunksTravelledThrough == std::numeric_limits<float>::max())){
				chunksTravelledThrough++;
				gameChunksTravelledTrough++;
			}
			distanceTravelled.x = cam.Position[0];
		}
		if(abs(distanceTravelled.y - cam.Position[1]) > 500){
			if(!(chunksTravelledThrough == std::numeric_limits<float>::max())){
				gameChunksTravelledTrough++;
				chunksTravelledThrough++;
			}
			if(cam.Position[1] < distanceTravelled.y)
				if(!(chunksFallenThrough == std::numeric_limits<int>::max())){
					chunksFallenThrough++;
					gameChunksTravelledTrough++;
				}
			distanceTravelled.y = cam.Position[1];
		}
	}


	//following line just makes sure that the attack timer is always added to
	if(!Keys[GLFW_KEY_SPACE] && player.timer >= 0.0001)
		player.canAttack(dt);

	//following line reduces the hit indicator for player
	if(player.isHit < 0){
		player.isHit = 0;
	}else if(player.isHit){
		player.isHit -= dt;
	}

	//adding to the attack timers of the enemies
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i]->attacking){
			board_enemies[i]->addAttackTimer(dt);
		}
	}

	if(m_state == GAME_ACTIVE_CLASSIC){
		if(abs(player.velocity.x) >= 0.01 || abs(player.velocity.y) >= 0.01){
			points += dt/2;
			enemyMultiplier += 0.004 * dt;
			player.experience += dt/20;
		}
		//moving the projectiles
		moveAllProjectiles(dt);

		if(points >= recoveryTimer + 70){
			player.health += player.recovery;
			player.health = std::min(player.health, maxHealth);
			playerHealth = player.health;
			recoveryTimer = points;
		}

		//update enemy hitData
		for(int i{}; i < board_enemies.size(); ++i){
			board_enemies[i]->decreaseHitDataTime(dt);
		}

		//testing if it is time to generate new chunks
		//0 up
		//1 right
		//2 down
		//3 left
		if(cam.Position[0] >= distanceFromCenterForGeneration){
			generateChunks(1);
			fixPlayerPosition(1);
		}else if(cam.Position[0] <= -distanceFromCenterForGeneration){
			generateChunks(3);
			fixPlayerPosition(3);
		}

		if(cam.Position[1] >= distanceFromCenterForGeneration){
			generateChunks(0);
			fixPlayerPosition(0);
		}else if(cam.Position[1] <= -distanceFromCenterForGeneration){
			generateChunks(2);
			fixPlayerPosition(2);
		}

		//finding out which chunk the player is in using position
		short width, height;
		//function returns which square the player is in
		findLocationCoordinates(width, height, cam.Position[0], cam.Position[1]);
		//collision detection between player and objects(the ground/plants/gates)
		nineBlockCollisionDetectionPAndO(width, height, dt);
		//process effects for enemies
		processEffectsForEnemies(dt);
		//process effects for player
		processEffectsForPlayer(dt);
		//collision detection between player projectiles and enemies
		//collision detection between the projectile and blocks
		player_projectile_collision_detection();
		//collision detection between enemy projectile and player
		//collision detection between the projectile and the blocks
		enemy_projectile_collision_detection();
		//looping to see if any enemies have died
		clearDeadEnemies();
		if(!completedAchievements[42])
			if(player.bowl->name.compare("crown") == 0 && std::find_if(player.plants.begin(), player.plants.end(), [](Plant p){return p.name.compare("dragonFruit") == 0 && p.level == 4;}) != player.plants.end())
				knightsStory = true;
		if(!completedAchievements[43])
			if(std::find_if(player.plants.begin(), player.plants.end(), [](Plant p){return p.name.compare("kale") == 0 && p.level == 4;}) != player.plants.end())
				kaleIsBad = true;
		if(!completedAchievements[44])
			if(std::find_if(player.plants.begin(), player.plants.end(), [](Plant p){return p.name.compare("kill") == 0 && p.level == 4;}) != player.plants.end())
				chillBois = true;


		moveEnemies(dt);
		//check if the player has died
		if(player.isDead()){
			gameEndProtocol();
		}
		enemyAttackLogic();
		moveBackground(dt);
		if(timeAlive >= 150 && noHealthLost)
			time_150_passed = true;
	}

	//test for the other states else if()
};

/**
calls correct rendering functions
*/
void Game::Render(){
	/*
	camera is already at center of the player for the collision detection part
	that means that the constant offset value for the player is
	-player->bowl.size[0]/2 and player->bowl.size[1]/2
	position for player is also changing every frame, calculated from camera

	screen is centered on the player
	player starts with at -player->bowl.size[0]/2 and player->bowl.size[1]/2
	meaning camera starts centered at 0,0 and on the player
	means that the horizontal edge of the starting screen is equal to
	-300 and 300
	vertical is then at 
	-400 and 400

	perception vector has been changed to match this change
	now runs from -300 to 300 horizontally and -400 to 400 vertically
	thus we can specify coordinates using this and have them translated for us
	*/

	switch(m_state){
		case HOME_MAIN:
			renderHomeMain();
			break;
		case GAME_ACTIVE_CLASSIC:
			renderGame();
			break;
		case START_SCREEN:
			renderStartScreen();
			break;
		case HOME_ARMORY:
			renderArmoryScreen();
			break;
		case HOME_ACHIEVEMENTS:
			renderAchievements();
			break;
		case DEATH_SCREEN:
			renderDeathScreen();
			break;
		case HOME_GREENHOUSE:
			renderGreenhouse();
			break;
	}
}
/*



GAME INITIALIZATION/DESTRUCTION





*/
/**
sets up the game to its initial state
*/
void Game::initializeGame(){
	generatedChunks = true;
	std::uniform_int_distribution chunkSelector{1,static_cast<int>(numOfChunks)};
	std::uniform_int_distribution random{1, 100};
	for(short k{}; k < 9; ++k){
		std::vector<Chunk> temp;
		//getting all the chunks
		for(short i{}; i < 100; ++i){
			temp.push_back(ResourceManager::GetChunk(std::to_string(chunkSelector(mersenne))));
		}
		//add spawning logic here
		for(short i{}; i < 100; ++i){
			for(short j{10}; j < 100; ++j){
				if(temp[i].locationOfObjects[j]){
					if(!temp[i].locationOfObjects[j-10]){
						short rnum = random(mersenne);
						if(rnum <= 5 + player.luck){
							spawnPlant(temp, i, j);
						}else if(rnum <= 30){
							spawnEnemy(i, j, generationCodes2[k], generationCode[k]);
							auto it = board_enemies.rbegin();
							(*it)->applyMultiplier(1.0);
						}
					}
				}
			}
		}
		board[k/3][k%3] = temp;
	}
}
/**
loads enemies from memory and adds them to the resourcemanager enemy map
*/
void Game::loadEnemies(){
	//create the basic enemy here except don't have position set
	//then save it to the map as a broad class enemy not
	//as the specific one
	//then they select it and set the position
	ResourceManager::setDepth("Melee", 0);
}
/**
clears the dequeues and vectors used to store the game environment
*/
void Game::clearAndResetGameBoard(){
	for(int i{}; i < 2; ++i){
		board.erase(board.begin());
	}
	for(int i{}; i < 3; ++i){
		std::deque<std::vector<Chunk>> temp;
		for(int j{}; j < 3; ++j){
			std::vector<Chunk> temp2(100);
			temp.push_back(temp2);
		}
		board.push_back(temp);
	}
}
/**
sets the background pointer to the correct background
@param: string background name
*/
void Game::setBackground(std::string &name){
	backgroundTextures = &ResourceManager::GetBackground(name);
}
/**
fills board storage device with empty arrays to make it easier to create the board
*/
void Game::prepBoard(){
	for(int i{}; i < 3; ++i){
		std::deque<std::vector<Chunk>> temp;
		for(int j{}; j < 3; ++j){
			std::vector<Chunk> temp2(100);
			temp.push_back(temp2);
		}
		board.push_back(temp);
	}
}
/**
runs after player dies
checks achievements and bowl unlocks
resets all game instance related variables
*/
void Game::gameEndProtocol(){
	clearAndResetGameBoard();
	addExpToGreenhousePlants();
	checkBowls();
	checkAchievements();

	player.calculateLevel();
	player.calculateStats();
	player.interact = nullptr;
	player.statBoosts.clear();
	player.plants.clear();
	player.effects.clear();
	player.velocity[0] = 0;
	player.velocity[1] = 0;

	board_enemies.clear();
	hitData.clear();
	enemy_projectiles.clear();
	player_projectiles.clear();
	player.statBoosts.clear();

	m_state = DEATH_SCREEN;

	deathScreenCoords[0] = cam.Position[0];
	deathScreenCoords[1] = cam.Position[1];
	cam.Position[0] = 0;
	cam.Position[1] = 0;

	enemyMultiplier = player.level/10.0;

	chunksFallenThrough = 0;
}
/**
starts the game
sets needed variables for game instance
*/
void Game::gameStartProtocol(){
	m_state = GAME_ACTIVE_CLASSIC;
	initializeGame();
	player.setStatBoosts();
	for(int i{}; i < selectedPlantTexCoords.size(); ++i){
		if(selectedPlantTexCoords[i] != numGreenHouse)
			player.setStatBoosts(greenhouse[selectedPlantTexCoords[i]].boost, greenhouse[selectedPlantTexCoords[i]].level);
	}
	player.setFinalStats();
	playerHealth = player.health;
	maxHealth = player.health;
	cam.Position[0] = 0;
	cam.Position[1] = 0;
	points = 0;
	recoveryTimer = 0;
	numEnemyKilled.clear();
	for(int i{}; i < numOfEnemies; ++i){
		numEnemyKilled.push_back(0);
	}
	gameChunksTravelledTrough = 0;
	plantsCollected = 0;
	if(!completedAchievements[38]){
		if(player.bowl->name.compare("basic") == 0)
			playedBowls[0] = true;
		else if(player.bowl->name.compare("box") == 0)
			playedBowls[1] = true;
		else if(player.bowl->name.compare("iv_bag") == 0)
			playedBowls[2] = true;
		else if(player.bowl->name.compare("lightbulb") == 0)
			playedBowls[3] = true;
		else if(player.bowl->name.compare("thanos") == 0)
			playedBowls[4] = true;
		else 
			std::cout << "could not find bowl for game data\n";
	}

	timeAlive = 0;
	noHealthLost = true;
	time_150_passed = false;
	timeStill = 0;
	previousPosition = glm::vec2(0.0);
	time_60_still_passed = false;
}

void Game::setUnlockedBowls(){
	std::string line;
	std::ifstream fstream("bin/directories/bowlUnlockFile.txt");
	if(fstream){
		while(std::getline(fstream, line)){
			bowls.push_back(line.compare("1") == 0);
			std::getline(fstream, line);
			bowlNames.push_back(line);
		}
	}else{
		std::cout << "unlock bowl file not fount\n";
	}
}

void Game::prepAchievementScreen(){
	std::string line;
	std::ifstream fstream("bin/directories/achievementUnlockFile.txt");
	if(fstream){
		while(std::getline(fstream, line)){
			completedAchievements.push_back(line.compare("1") == 0);
			std::getline(fstream, line);
			achievementNames.push_back(line);
		}
	}else{
		std::cout << "unlock achievement file not found\n";
	}
	for(int i{}; i < 49; ++i){
		glm::vec2 temp{(i%7) * 55.0 / 45.0, -((i/7) * 55.0 / 45.0)};
		achievementOffsets.push_back(temp);

		if(completedAchievements[i])
			achievementTexCoords.push_back(i);
		else
			achievementTexCoords.push_back(0);
	}
}

void Game::loadGameData(){
	std::string line;
	std::ifstream fstream("bin/directories/gameData.txt");
	if(fstream){
		std::getline(fstream, line);
		chunksTravelledThrough = std::stof(line);
		
		for(int i{}; i < 6; ++i){
			std::getline(fstream, line);
			selectedPlantTexCoords.push_back(std::stof(line));
		}
		//tempcode
		for(int i{}; i < 5; ++i){
			playedBowls.push_back(false);
		}
	}else{
		std::cout << "game data file not found\n";
	}
}

void Game::prepGreenhouse(){
	greenhouseLevelOffset.push_back(glm::vec2(0.0f,0.0f));
	std::string line;
	std::ifstream fstream("bin/directories/greenhouseUnlockFile.txt");
	if(fstream){
		std::string plantName, boostName;
		std::vector<float> boosts{};
		float level, experience;
		while(std::getline(fstream, line)){
			unlockedPlants.push_back(line.compare("1") == 0);
			std::getline(fstream, plantName);
			std::getline(fstream, boostName);
			for(int i{}; i < 7; ++i){
				std::getline(fstream, line);
				boosts.push_back(std::stof(line));
			}
			std::getline(fstream, line);
			level = std::stof(line);
			std::getline(fstream, line);
			experience = std::stof(line);
			greenhouse.push_back(greenHousePlant(boosts, boostName, plantName, level, experience));
			boosts.clear();
		}
	}else{
		std::cout << "unlock greenhouse file not found\n";
	}

	for(int i{}; i < 50; ++i){
		greenhouseOffsets.push_back(glm::vec2((i%10) * 60.0 / 55.0, -((i/10) * 60.0 / 55.0)));
		if(unlockedPlants[i])
			greenhouseTexCoords.push_back(ResourceManager::GetDepth(greenhouse[i].boostName));
		else
			greenhouseTexCoords.push_back();
	}
	for(int i{}; i < 6; ++i){
		selectedPlantOffset.push_back(glm::vec2((i%3) * 60.0 / 55.0, -((i/3) * 60.0 / 55.0)));
	}
}

void Game::greenhouseSelectorHelper(int avoid){
	bool canSelect = true;
	for(int i{0}; i < 6; ++i){
		if(i != avoid && static_cast<int>(selectedPlantTexCoords[i]) == plantSelector)
			canSelect = false;
	}
	if(canSelect){
		selectedPlantTexCoords[avoid] = plantSelector;
	}
}
/*

UNLOCK CHECKERS





*/
void Game::addExpToGreenhousePlants(){
	for(int i{}; i < selectedPlantTexCoords.size(); ++i){
		if(selectedPlantTexCoords[i] != numGreenHouse)
			greenhouse[selectedPlantTexCoords[i]].addExperience(points);
	}
	setGreenhouseTextureValues();
}
void Game::checkBowls(){
	if(time_150_passed && points >= 500)
		bowls[2] = true;
	
	if(time_60_still_passed)
		bowls[1] = true;

	if(std::accumulate(numEnemyKilled.begin(), numEnemyKilled.end(), 0) >= 800)
		bowls[3] = true;

	if(lookedAtCredits)
		bowls[4] = true;
}

void Game::checkAchievements(){
	//distance achievements
	if(chunksTravelledThrough >= 500000)
		setAchievementsToTrue(0, 9);
	else if(chunksTravelledThrough >= 119450)
		setAchievementsToTrue(0, 8);
	else if(chunksTravelledThrough >= 42165)
		setAchievementsToTrue(0, 7);
	else if(chunksTravelledThrough >= 24901)
		setAchievementsToTrue(0, 6);
	else if(chunksTravelledThrough >= 15426)
		setAchievementsToTrue(0, 5);
	else if(chunksTravelledThrough >= 10000)
		setAchievementsToTrue(0, 4);
	else if(chunksTravelledThrough >= 2000)
		setAchievementsToTrue(0, 3);
	else if(chunksTravelledThrough >= 500)
		setAchievementsToTrue(0, 2);
	else if(chunksTravelledThrough >= 100)
		setAchievementsToTrue(0, 1);
	else if(chunksTravelledThrough >= 5)
		setAchievementsToTrue(0, 0);

	if(chunksFallenThrough >= 100)
		setAchievementsToTrue(11, 13);
	else if(chunksFallenThrough >= 24)
		setAchievementsToTrue(11, 12);
	else if(chunksFallenThrough >= 5)
		setAchievementsToTrue(11, 11);

	if(gameChunksTravelledTrough >= 200)
		setAchievementsToTrue(10, 10);

	//enemykilled achievements
	float sum = std::accumulate(numEnemyKilled.begin(), numEnemyKilled.end(), 0);
	if(sum >= 100000)
		setAchievementsToTrue(14, 22);
	else if(sum >= 20000)
		setAchievementsToTrue(14, 21);
	else if(sum >= 5000)
		setAchievementsToTrue(14, 20);
	else if(sum >= 2000)
		setAchievementsToTrue(14, 19);
	else if(sum >= 400)
		setAchievementsToTrue(14, 18);
	else if(sum >= 150)
		setAchievementsToTrue(14, 17);
	else if(sum >= 50)
		setAchievementsToTrue(14, 16);
	else if(sum >= 10)
		setAchievementsToTrue(14, 15);
	else if(sum >= 1)
		setAchievementsToTrue(14, 14);

	if(std::all_of(numEnemyKilled.begin(), numEnemyKilled.end(), [](int i){return i> 5;}))
		setAchievementsToTrue(23, 24);
	else if(std::all_of(numEnemyKilled.begin(), numEnemyKilled.end(), [](int i){return i> 1;}))
		setAchievementsToTrue(23, 23);

	if(std::find_if(numEnemyKilled.begin(), numEnemyKilled.end(), [](int i){return i > 500;}) != numEnemyKilled.end())
		setAchievementsToTrue(25, 27);
	else if(std::find_if(numEnemyKilled.begin(), numEnemyKilled.end(), [](int i){return i > 200;}) != numEnemyKilled.end())
		setAchievementsToTrue(25, 26);
	else if(std::find_if(numEnemyKilled.begin(), numEnemyKilled.end(), [](int i){return i > 100;}) != numEnemyKilled.end())
		setAchievementsToTrue(25, 25);

	//collection achievements
	int unlockedPlantsCount = std::count(unlockedPlants.begin(), unlockedPlants.end(), true);
	if(unlockedPlantsCount == 50)
		setAchievementsToTrue(28, 31);
	else if(unlockedPlantsCount >= 25)
		setAchievementsToTrue(28, 30);
	else if(unlockedPlantsCount >= 10)
		setAchievementsToTrue(28, 29);
	else if(unlockedPlantsCount >= 5)
		setAchievementsToTrue(28, 28);

	int maxedLevelPlants = std::count_if(greenhouse.begin(), greenhouse.end(), [](greenHousePlant p){return p.level == 9;});
	if(maxedLevelPlants == 50)
		setAchievementsToTrue(32, 35);
	else if(maxedLevelPlants >= 10)
		setAchievementsToTrue(32, 34);
	else if(maxedLevelPlants >= 5)
		setAchievementsToTrue(32, 33);
	else if(maxedLevelPlants >= 1)
		setAchievementsToTrue(32, 32);

	if(std::count(bowls.begin(), bowls.end(), true) == 5)
		setAchievementsToTrue(36, 37);
	else if(std::count(bowls.begin(), bowls.end(), true) >= 2)
		setAchievementsToTrue(36, 36);

	if(std::count(playedBowls.begin(), playedBowls.end(), true) == 5)
		setAchievementsToTrue(38, 38);

	if(completedAchievements[37] && completedAchievements[31])
		setAchievementsToTrue(39, 39);

	if(plantsCollected >= 50)
		setAchievementsToTrue(40, 41);
	else if(plantsCollected >= 10)
		setAchievementsToTrue(40, 40);


	//end game/random achievements
	if(knightsStory)
		setAchievementsToTrue(42, 42);
	if(kaleIsBad)
		setAchievementsToTrue(43, 43);
	if(chillBois)
		setAchievementsToTrue(44, 44);

	if(points >= 100000)
		setAchievementsToTrue(45, 47);
	else if(points >= 10000)
		setAchievementsToTrue(45, 46);
	else if(points >= 1000)
		setAchievementsToTrue(45, 45);

	if(std::count(completedAchievements.begin(), completedAchievements.end() - 1, true) == 48)
		setAchievementsToTrue(48, 48);

	setAchievementTextureValues();
}

void Game::setAchievementsToTrue(int start, int stop){
	for(int i = start; i < stop + 1; ++i){
		if(!completedAchievements[i])
			completedAchievements[i] = true;
	}
}

void Game::setAchievementTextureValues(){
	achievementTexCoords.clear();
	for(int i{}; i < completedAchievements.size(); ++i){
		if(completedAchievements[i]){
			achievementTexCoords.push_back(i);
		}else{
			achievementTexCoords.push_back(numAchievements);
		}
	}
}

void Game::setGreenhouseTextureValues(){
	greenhouseTexCoords.clear();
	for(int i{}; i < unlockedPlants.size(); ++i){
		if(unlockedPlants[i])
			greenhouseTexCoords.push_back(i);
		else
			greenhouseTexCoords.push_back(numPlants);
	}
}
/*

CHUNK GENERATION





*/
void Game::prepBoardForChunkCreation(const short direction){
	//0 is up
	//1 is right
	//2 is down
	//3 is left
	std::deque<std::vector<Chunk>> temp;
	switch(direction){	
		case 0 :
			board.pop_back();
			for(int j{}; j < 3; ++j){
				std::vector<Chunk> temp2(100);
				temp.push_back(temp2);
			}
			board.push_front(temp);
			break;
		case 1:
			board[0].pop_front();
			board[1].pop_front();
			board[2].pop_front();
			break;
		case 2:
			board.pop_front();
			for(int j{}; j < 3; ++j){
				std::vector<Chunk> temp2(100);
				temp.push_back(temp2);
			}
			board.push_back(temp);
			break;
		case 3:
			board[0].pop_back();
			board[1].pop_back();
			board[2].pop_back();
			break;
	}
}

void Game::generateChunks(const short direction){
	generatedChunks = true;
	prepBoardForChunkCreation(direction);
	despawnEnemiesFromDeletedChunks(direction);
	fixRemainingEnemyPosition(direction);
	despawnProjectilesFromDeletedChunks(direction);
	fixRemainingProjectilePosition(direction);

	std::uniform_int_distribution chunkSelector{1,static_cast<int>(numOfChunks)};
	std::uniform_int_distribution random{1, 100};

	for(short k{}; k < 3; ++k){
		std::vector<Chunk> temp;
		//getting all the chunks
		for(short i{}; i < 100; ++i){
			temp.push_back(ResourceManager::GetChunk(std::to_string(chunkSelector(mersenne))));
		}
		//add spawning logic here
		for(short i{}; i < 100; ++i){
			for(short j{10}; j < 100; ++j){
				if(temp[i].locationOfObjects[j]){
					if(!temp[i].locationOfObjects[j-10]){
						short rnum = random(mersenne);
						if(rnum <= 5 + player.luck){
							spawnPlant(temp, i, j);
						}else if(rnum <= 30){
							spawnEnemy(i, j, k, direction);
							auto it = board_enemies.rbegin();
							(*it)->applyMultiplier(enemyMultiplier);
						}
					}
				}
			}
		}
		switch(direction){
			case 0 :
				board[0][k] = temp;
				break;
			case 1:
				board[k].push_back(temp);
				break;
			case 2:
				board[2][k] = temp;
				break;
			case 3:
				board[k].push_front(temp);
				break;
		}
	}
}

void Game::despawnEnemiesFromDeletedChunks(const short direction){
	//0 is up
	//1 is right
	//2 is down
	//3 is left
	short min_x, max_x, min_y, max_y;
	switch(direction){
		case 0 :
			min_y = -edgeDistace;
			max_y = -innerRegionEdgeDistance;
			min_x = -edgeDistace;
			max_x = edgeDistace;
			break;
		case 1 :
			min_y = -edgeDistace;
			max_y = edgeDistace;
			min_x = -edgeDistace;
			max_x = -innerRegionEdgeDistance;
			break;
		case 2 :
			min_y = innerRegionEdgeDistance;
			max_y = edgeDistace;
			min_x = -edgeDistace;
			max_x = edgeDistace;
			break;
		case 3 :
			min_y = -edgeDistace;
			max_y = edgeDistace;
			min_x = innerRegionEdgeDistance;
			max_x = edgeDistace;
			break;
	}
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i]->position[0] >= min_x && board_enemies[i]->position[0] <= max_x 
			&& board_enemies[i]->position[1] >= min_y && board_enemies[i]->position[1] <= max_y){
			delete board_enemies[i];
			board_enemies.erase(board_enemies.begin() + i);
			--i;
		}
	}
}

void Game::despawnProjectilesFromDeletedChunks(const short direction){
	//0 is up
	//1 is right
	//2 is down
	//3 is left
	short min_x, max_x, min_y, max_y;
	switch(direction){
		case 0 :
			min_y = -edgeDistace;
			max_y = -innerRegionEdgeDistance;
			min_x = -edgeDistace;
			max_x = edgeDistace;
			break;
		case 1 :
			min_y = -edgeDistace;
			max_y = edgeDistace;
			min_x = -edgeDistace;
			max_x = -innerRegionEdgeDistance;
			break;
		case 2 :
			min_y = innerRegionEdgeDistance;
			max_y = edgeDistace;
			min_x = -edgeDistace;
			max_x = edgeDistace;
			break;
		case 3 :
			min_y = -edgeDistace;
			max_y = edgeDistace;
			min_x = innerRegionEdgeDistance;
			max_x = edgeDistace;
			break;
	}
	for(int i{}; i < enemy_projectiles.size(); ++i){
		if(enemy_projectiles[i].position[0] >= min_x && enemy_projectiles[i].position[0] <= max_x 
			&& enemy_projectiles[i].position[1] >= min_y && enemy_projectiles[i].position[1] <= max_y){
			enemy_projectiles.erase(enemy_projectiles.begin() + i);
			--i;
		}
	}
	for(int i{}; i < player_projectiles.size(); ++i){
		if(player_projectiles[i].position[0] >= min_x && player_projectiles[i].position[0] <= max_x 
			&& player_projectiles[i].position[1] >= min_y && player_projectiles[i].position[1] <= max_y){
			player_projectiles.erase(player_projectiles.begin() + i);
			--i;
		}
	}
}

void Game::fixGeneratedEnemiesPosition(const short i, const short j, const short k, const short direction){
	Enemy* end = *(board_enemies.rbegin());
	//i is the mini chunk inside the larger chunk
	//i/10 gives the row
	//i%10 gives the column
	//
	//j is the location of the block inside the mini chunk
	//j/10 gives the row
	//j%10 gives the column
	//
	//k is the chunk being generated
	end->position.x = ((i%10)-5)*chunkSize + (((j-10)%10) * blockSize) + ((blockSize-maxEnemySize)/2);
	end->position.y = (5-(i/10))*chunkSize - (((j-10)/10) * blockSize) - (blockSize-maxEnemySize);

	switch(direction){
		case 0 :
			end->position.y += regionSize;
			if(k == 0)
				end->position.x -= regionSize;
			else if(k == 2)
				end->position.x += regionSize;
			break;
		case 1 :
			end->position.x += regionSize;
			if(k == 0)
				end->position.y += regionSize;
			else if(k == 2)
				end->position.y -= regionSize;
			break;
		case 2 :
			end->position.y -= regionSize;
			if(k == 0)
				end->position.x -= regionSize;
			else if(k == 2)
				end->position.x += regionSize;
			break;
		case 3 :
			end->position.x -= regionSize;
			if(k == 0)
				end->position.y += regionSize;
			else if(k == 2)
				end->position.y -= regionSize;
			break;
	}
}

void Game::fixRemainingEnemyPosition(const short direction){
	//0 up
	//1 right
	//2 down
	//3 left
	bool vert = direction % 2 == 0;
	int adder = findAddingAmountOffsetWhenGeneratingChunks(direction);
	for(int i{}; i < board_enemies.size(); ++i){
		if(vert)
			board_enemies[i]->position[1] += adder;
		else
			board_enemies[i]->position[0] += adder;
	}
}

void Game::fixRemainingProjectilePosition(const short direction){
	bool vert = direction % 2 == 0;
	short adder = findAddingAmountOffsetWhenGeneratingChunks(direction);
	for(int i{}; i < player_projectiles.size(); ++i){
		if(vert){
			player_projectiles[i].position[1] += adder;
		}else{
			player_projectiles[i].position[0] += adder;
		}
	}
	for(int i{}; i < enemy_projectiles.size(); ++i){
		if(vert){
			enemy_projectiles[i].position[1] += adder;
		}else{
			enemy_projectiles[i].position[0] += adder;
		}
	}
}

short Game::findAddingAmountOffsetWhenGeneratingChunks(const short direction){
	switch(direction){
		case 2 :
		case 3 :
			return regionSize;
		case 0 :
		case 1 :
			return -regionSize;
	}
	return 0;
}

void Game::fixPlayerPosition(const short direction){
	switch(direction){
		case 0:
			cam.Position[1] -= regionSize;
			break;
		case 1:
			cam.Position[0] -= regionSize;
			break;
		case 2:
			cam.Position[1] += regionSize;
			break;
		case 3:
			cam.Position[0] += regionSize;
			break;
	}
}

void Game::spawnPlant(std::vector<Chunk> &temp, short i, short j){
	std::uniform_int_distribution plantPicker{0, static_cast<int>(numOfPlants) - 1};
	//5% chance to spawn in a plant
	//uses the plant object but puts a location
	//copies non-important data into the stuff
	int plantNum = plantPicker(mersenne);
	auto it = ResourceManager::Plants.begin();
	while(plantNum--){
		++it;
	}
	temp[i].plants.push_back((it)->second);
	//position also accounts for the plant being smaller than the block
	//thus it would be offset a little bit
	auto end = temp[i].plants.rbegin();
	end->position.x += ((j-10)%10) * blockSize;
	end->position.y -= ((j-10)/10) * blockSize;
}

void Game::spawnEnemy(short i, short j, short k, short l){
	std::uniform_int_distribution enemyPicker{0, static_cast<int>(numOfEnemies) - 1};
	//20% chance to spawn an enemy
	//stored inside of a vector for the Game class
	//might change this to have a certain percentage per enemy
	//and then spawn that enemy in
	int enemyNum = enemyPicker(mersenne);
	if(enemyNum == 0){
		Enemy *temp = new Melee();
		board_enemies.push_back(temp);
	}
	fixGeneratedEnemiesPosition(i, j, k, l);
}
/*




COLLISION DETECTION




*/
void Game::player_and_object_collisions(GameObject *object, const float dt, const short gameobject_offset_x, const short gameobject_offset_y){
	
	bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
		&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
	bool collisionY = cam.Position[1] - player.bowl->size[1]/2 <= object->position[1] + gameobject_offset_y
		&& object->position[1] - object->size[1] + gameobject_offset_y <= cam.Position[1] + player.bowl->size[1]/2;

	if(collisionX && collisionY){
		//getting previous location
		short direction = findPlayerDirection(object, dt, gameobject_offset_x, gameobject_offset_y);
		//applying the corrections to the players position
		//while also fixing the velocity in that direction
		//to make it seem like they were stopped by the object
		switch(direction){
			case 0:
				cam.Position[1] += (object->position[1] + gameobject_offset_y) - (cam.Position[1] - player.bowl->size[1]/2);
				player.velocity[1] = 0.0f;
				player.falling = false;
				break;
			case 1:
				cam.Position[0] += object->position[0] + object->size[0] + gameobject_offset_x - (cam.Position[0] - player.bowl->size[0]/2);
				player.velocity[0] = 0.0f;
				break;
			case 2:
				cam.Position[1] -= (cam.Position[1] + player.bowl->size[1]/2) - (object->position[1] - object->size[1] + gameobject_offset_y);
				player.velocity[1] = 0.0f;
				break;
			case 3:
				cam.Position[0] -= cam.Position[0] + player.bowl->size[0]/2 - (object->position[0] + gameobject_offset_x);
				player.velocity[0] = 0.0f;
				break;
			default:
				break;
		}
	}

}

void Game::player_and_object_collisions(GameObject *object, const float dt, const short index_x, const short index_y, const short index_chunk, const short gameobject_offset_x, const short gameobject_offset_y){
	if(object->interactable){
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
			&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] - player.bowl->size[1]/2 <= object->position[1] + gameobject_offset_y
			&& object->position[1] - object->size[1] + gameobject_offset_y <= cam.Position[1] + player.bowl->size[1]/2;
		if(collisionX && collisionY){
			//set the object as the interactable object
			player.interact = object;
			player.location = findInteractPosition(object, index_x, index_y, index_chunk);
		}
	}else{
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
			&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] - player.bowl->size[1]/2 <= object->position[1] + gameobject_offset_y
			&& object->position[1] - object->size[1] + gameobject_offset_y <= cam.Position[1] + player.bowl->size[1]/2;

		if(collisionX && collisionY){
			//getting previous location
			short direction = findPlayerDirection(object, dt, gameobject_offset_x, gameobject_offset_y);
			//applying the corrections to the players position
			//while also fixing the velocity in that direction
			//to make it seem like they were stopped by the object
			switch(direction){
				case 0:
					cam.Position[1] += (object->position[1] + gameobject_offset_y) - (cam.Position[1] - player.bowl->size[1]/2);
					player.velocity[1] = 0.0f;
					player.falling = false;
					break;
				case 1:
					cam.Position[0] += object->position[0] + object->size[0] + gameobject_offset_x - (cam.Position[0] - player.bowl->size[0]/2);
					player.velocity[0] = 0.0f;
					break;
				case 2:
					cam.Position[1] -= (cam.Position[1] + player.bowl->size[1]/2) - (object->position[1] - object->size[1] + gameobject_offset_y);
					player.velocity[1] = 0.0f;
					break;
				case 3:
					cam.Position[0] -= cam.Position[0] + player.bowl->size[0]/2 - (object->position[0] + gameobject_offset_x);
					player.velocity[0] = 0.0f;
					break;
				default:
					break;
			}
		}
	}
}

bool Game::game_classic_two_object_collisions(GameObject *object, GameObject *projectile){
	bool collisionX =  projectile->position[0] + projectile->size[0] >= object->position[0]
		&& object->position[0] + object->size[0] >= projectile->position[0];
	bool collisionY = projectile->position[1] - projectile->size[1] <= object->position[1]
		&& object->position[1] - object->size[1] <= projectile->position[1];
	return (collisionX && collisionY);
}

bool Game::game_classic_two_object_collisions(GameObject *object, GameObject *object2, const short gameobject_offset_x, const short gameobject_offset_y){
	bool collisionX =  object2->position[0] + object2->size[0] >= object->position[0] + gameobject_offset_x
		&& object->position[0] + object->size[0] + gameobject_offset_x >= object2->position[0];
	bool collisionY = object2->position[1] - object2->size[1] <= object->position[1] + gameobject_offset_y
		&& object->position[1] - object->size[1] + gameobject_offset_y <= object2->position[1];
	return (collisionX && collisionY);
}

bool Game::game_classic_two_object_collisions(glm::vec2 position, glm::vec2 size, GameObject *projectile){
	bool collisionX =  projectile->position[0] + projectile->size[0] >= position[0]
		&& position[0] + size[0] >= projectile->position[0];
	bool collisionY = projectile->position[1] - projectile->size[1] <= position[1]
		&& position[1] - size[1] <= projectile->position[1];
	return (collisionX && collisionY);
}

void Game::findLocationCoordinates(short &width, short &height, const float x, const float y){
	//this changes the variables to the index
	//no returns, just changes the variables passed to it
	if(y / chunkSize == 0){
		if(y < 0){
			height = -1;
		}else{
			height = 0;
		}
	}else{
		if(y < 0){
			height = y/chunkSize -1;
		}else{
			height = y/ chunkSize;
		}
	}
	if(x / chunkSize == 0){
		if(x < 0){
			width = -1;
		}else{
			width = 0;
		}
	}else{
		if(x < 0){
			width = x/chunkSize -1;
		}else{
			width = x/ chunkSize;
		}
	}
}

void Game::nineBlockCollisionDetectionPAndO(const short width, const short height, const float dt){
	//finding the chunks right next to the main chunk
	short chunk_width_offset = width == -15 ? 0 : -1;
	short max_width_chunk_offset = width == 14 ? 0 : 1;
	short chunk_height_offset = height == -15 ? 0 : -1;
	short max_height_chunk_offset = height == 14 ? 0 : 1;
	//iterating through those chunks
	for(; chunk_width_offset <= max_width_chunk_offset; chunk_width_offset++){
		//figuring out which chunk on the board it corresponds to
		short index_x, index_y, index_chunk{};
		if(width + chunk_width_offset <= -6){
			index_y = 0;
			index_chunk = width + chunk_width_offset + 15;
		}else if(width + chunk_width_offset <= 4){
			index_y = 1;
			index_chunk = width + chunk_width_offset + 5;
		}else{
			index_y = 2;
			index_chunk = width + chunk_width_offset - 5;
		}
		for(int i = chunk_height_offset; i <= max_height_chunk_offset; i++){
			short index_chunk_2;
			if(height + i <= -6){
				index_x = 2;
				index_chunk_2 = (-6 - (height + i)) * 10;
			}else if(height + i <= 4){
				index_x = 1;
				index_chunk_2 = (4 - (height + i)) * 10;
			}else{
				index_x = 0;
				index_chunk_2 = (14 - (height + i)) * 10;
			}

			//check this chunk and loop through the gameobjects
			//detect collisions between them and the player object
			//the height offset needed for each chunk is equal to the (y coordinate + 1) of the location multiplied by 500
			//the width offset needed for each chunk is equal to the x coordinate of the location multiplied by 500
			short gameobject_offset_y = (height + 1 + i) * chunkSize;
			short gameobject_offset_x = (width + chunk_width_offset) * chunkSize;
			for(int j{}; j < board[index_x][index_y][index_chunk + index_chunk_2].objects.size(); ++j){
				//loop through all of the blocks in the chunk
				player_and_object_collisions(board[index_x][index_y][index_chunk + index_chunk_2].objects[j], dt, gameobject_offset_x, gameobject_offset_y);
			}
			for(int j{}; j < board[index_x][index_y][index_chunk + index_chunk_2].plants.size(); ++j){
				//loop through all of the plants in the chunk
				player_and_object_collisions((GameObject *)&(board[index_x][index_y][index_chunk + index_chunk_2].plants[j]), dt, index_x, index_y, index_chunk + index_chunk_2, gameobject_offset_x, gameobject_offset_y);
			}
		}
	}
}

bool Game::nineBlockCollisionDetectionGeneral(const short width, const short height, GameObject *object){
	//finding the chunks right next to the main chunk
	short chunk_width_offset = width == -15 ? 0 : -1;
	short max_width_chunk_offset = width == 14 ? 0 : 1;
	short chunk_height_offset = height == -15 ? 0 : -1;
	short max_height_chunk_offset = height == 14 ? 0 : 1;
	//iterating through those chunks
	for(; chunk_width_offset <= max_width_chunk_offset; chunk_width_offset++){
		//figuring out which chunk on the board it corresponds to
		short index_x, index_y, index_chunk;
		if(width + chunk_width_offset <= -6){
			index_y = 0;
			index_chunk = width + chunk_width_offset + 15;
		}else if(width + chunk_width_offset <= 4){
			index_y = 1;
			index_chunk = width + chunk_width_offset + 5;
		}else{
			index_y = 2;
			index_chunk = width + chunk_width_offset - 5;
		}
		for(int i = chunk_height_offset; i <= max_height_chunk_offset; i++){
			short index_chunk_2;
			if(height + i <= -6){
				index_x = 2;
				index_chunk_2 = (-6 - (height + i)) * 10;
			}else if(height + i <= 4){
				index_x = 1;
				index_chunk_2 = (4 - (height + i)) * 10;
			}else{
				index_x = 0;
				index_chunk_2 = (14 - (height + i)) * 10;
			}

			//check this chunk and loop through the gameobjects
			//detect collisions between them and the player object
			//the height offset needed for each chunk is equal to the (y coordinate + 1) of the location multiplied by 500
			//the width offset needed for each chunk is equal to the x coordinate of the location multiplied by 500
			short gameobject_offset_y = (height + 1 + i) * chunkSize;
			short gameobject_offset_x = (width + chunk_width_offset) * chunkSize;
			for(int j{}; j < board[index_x][index_y][index_chunk + index_chunk_2].objects.size(); ++j){
				//loop through all of the blocks in the chunk
				if(game_classic_two_object_collisions(board[index_x][index_y][index_chunk + index_chunk_2].objects[j], object, gameobject_offset_x, gameobject_offset_y))
					return true;
			}
		}
	}
	return false;
}

void Game::player_projectile_collision_detection(){
	short width, height;
	for(int i{}; i < player_projectiles.size(); ++i){
		bool deletionTracker{false};
		for(int j{}; j < board_enemies.size(); ++j){
			if(abs(board_enemies[j]->position[0] - player_projectiles[i].position[0]) > 500){
				if(board_enemies[j]->hitByPiercing)
					board_enemies[j]->hitByPiercing = false;
				continue;
			}else if(abs(board_enemies[j]->position[1] - player_projectiles[i].position[1]) > 500){
				if(board_enemies[j]->hitByPiercing)
					board_enemies[j]->hitByPiercing = false;
				continue;
			}else
				if(game_classic_two_object_collisions((GameObject *)((board_enemies[j])), (GameObject *)&(player_projectiles[i]))){
					//deal damage
					if(!player_projectiles[i].piercing || !board_enemies[j]->hitByPiercing){
						board_enemies[j]->health -= std::max(player_projectiles[i].damage + player.attack - board_enemies[j]->defense, 1);
					}
					//add effects
					board_enemies[j]->addEffects(player_projectiles[i]);
					board_enemies[j]->hitData = 0.2;
					if(!player_projectiles[i].piercing){
						player_projectiles.erase(player_projectiles.begin() + i);
						--i;
						deletionTracker = true;
						break;
					}else if(player_projectiles[i].piercing){
						board_enemies[j]->hitByPiercing = true;
					}
				}else if(board_enemies[j]->hitByPiercing)
					board_enemies[j]->hitByPiercing = false;
		}
		//collision detection between the projectile and the edge
		if(!deletionTracker){
			if(abs(player_projectiles[i].position[0]) > edgeDistace || abs(player_projectiles[i].position[1]) > edgeDistace){
				player_projectiles.erase(player_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
		//collision detection between the projectile and blocks
		if(!deletionTracker){
			findLocationCoordinates(width, height, player_projectiles[i].position[0], player_projectiles[i].position[1]);
			if(nineBlockCollisionDetectionGeneral(width, height, (GameObject *)&(player_projectiles[i]))){
				player_projectiles.erase(player_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
		//checking range for the projectile
		if(!deletionTracker){
			if(player_projectiles[i].rangeCheck()){
				player_projectiles.erase(player_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
	}
}

void Game::enemy_projectile_collision_detection(){
	short width, height;
	for(int i{}; i < enemy_projectiles.size(); ++i){
		bool deletionTracker{false};
		if(abs(cam.Position[0] - enemy_projectiles[i].position[0]) > 500)
			continue;
		else if(abs(cam.Position[1] - enemy_projectiles[i].position[1]) > 500)
			continue;
		else
			if(game_classic_two_object_collisions(glm::vec2(cam.Position[0]-player.bowl->size[0]/2, cam.Position[1]+player.bowl->size[1]/2), player.bowl->size, (GameObject *)&(enemy_projectiles[i]))){
				player.dealDamage(enemy_projectiles[i].damage);
				player.addEffects(enemy_projectiles[i]);
				enemy_projectiles.erase(enemy_projectiles.begin() + i);
				--i;
				deletionTracker = true;
				noHealthLost = false;
			}
		//collision detection between the projectile and the edge
		if(!deletionTracker){
			if(abs(enemy_projectiles[i].position[0]) > edgeDistace || abs(enemy_projectiles[i].position[1]) > edgeDistace){
				enemy_projectiles.erase(enemy_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
		//collision detection between the projectile and the blocks
		if(!deletionTracker){
			findLocationCoordinates(width, height, enemy_projectiles[i].position[0], enemy_projectiles[i].position[1]);
			if(nineBlockCollisionDetectionGeneral(width, height, (GameObject *)&(enemy_projectiles[i]))){
				enemy_projectiles.erase(enemy_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
		//checking range for the projectile
		if(!deletionTracker){
			if(enemy_projectiles[i].rangeCheck()){
				enemy_projectiles.erase(enemy_projectiles.begin() + i);
				--i;
				deletionTracker = true;
			}
		}
	}
}

short Game::findPlayerDirection(GameObject *object, const float dt, const short gameobject_offset_x, const short gameobject_offset_y){
	short direction;
	glm::vec2 prevPosition;
	prevPosition[0] = cam.Position[0] - player.velocity[0] * dt;
	prevPosition[1] = cam.Position[1] - player.velocity[1] * dt;
	//finding which side the player is hitting the block
	//0 is top side
	//1 is right side
	//2 is bottom side
	//3 is left side
			
	//testing when player is above the object
	if(prevPosition[1] - player.bowl->size[1]/2 >= object->position[1] + gameobject_offset_y){
		//testing to see if the player with clearly above the block
		if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) &&  prevPosition[0] - (object->position[0] + gameobject_offset_x + object->size[0]) < player.bowl->size[0]/2)
			direction = 0;
	}

	//testing when the player is below the object
	else if(prevPosition[1] + player.bowl->size[1]/2 <= object->position[1] - object->size[1] + gameobject_offset_y){
		//testing to see if the player with clearly above the block
		if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) && prevPosition[0] - (object->position[0] + gameobject_offset_x + object->size[0]) < player.bowl->size[0]/2)
			direction = 2;
	}
	//testing when the rest of the situations where the block approaches from the side
	//previous ifs have eliminated the top and bottom approaches
	else if(prevPosition[0] + player.bowl->size[0]/2 <= object->position[0] + gameobject_offset_x){
		direction = 3;
	}else if(prevPosition[0] - player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x + object->size[0]){
		direction = 1;
	}else{
		direction = -1;
	}
	return direction;
}

glm::vec4 Game::findInteractPosition(GameObject *object, short index_x, short index_y, short index_chunk){
	short width, height;
	glm::vec4 output = {index_x, index_y, index_chunk, 0.0f};
	for(auto it = board[output[0]][output[1]][output[2]].plants.begin(); it != board[output[0]][output[1]][output[2]].plants.end(); ++it){
		if(it->position == object->position){
			output[3] = it - board[output[0]][output[1]][output[2]].plants.begin();
			break;
		}
	}
	return output;
}
/*





GAME LOGIC






*/
void Game::spawnPlayerProjectile(){
	if(player.currentPlant == -1){
		player_projectiles.push_back(ResourceManager::GetProjectile("basic"));
		auto it = player_projectiles.rbegin();
		glm::vec2 startPosition = getProjectileStartPositionForPlayer(*it);
		short direction = player.facing ? 1 : -1;
		it->setDirection(startPosition, direction);
		it->right = true;
	}else{
		std::string projectileName = player.plants[player.currentPlant].projectileName[player.plants[player.currentPlant].level-1];
		if(projectileName.compare("cactus4") == 0){
			player_projectiles.push_back(ResourceManager::GetProjectile("cactus4"));
			auto it = player_projectiles.rbegin();
			glm::vec2 startPosition = getProjectileStartPositionForPlayer(*it);
			startPosition[1] += 20;
			short direction = player.facing ? 1 : -1;
			it->setDirection(startPosition, direction);
			it->right = player.facing;
			player_projectiles.push_back(ResourceManager::GetProjectile("cactus4"));
			it = player_projectiles.rbegin();
			startPosition[1] -= 30;
			it->setDirection(startPosition, direction);
			it->right = player.facing;
		}else{
			player_projectiles.push_back(ResourceManager::GetProjectile(projectileName));
			auto it = player_projectiles.rbegin();
			glm::vec2 startPosition = getProjectileStartPositionForPlayer(*it);
			short direction = player.facing ? 1 : -1;
			it->setDirection(startPosition, direction);
			it->right = player.facing;
		}
	}
}

glm::vec2 Game::getProjectileStartPositionForPlayer(Projectile &p){
	glm::vec2 output;
	//if player.facing is true, it is firing to the right
	if(player.facing){
		output[0] = cam.Position[0] + player.bowl->size[0]/2;
		output[1] = cam.Position[1] + p.size[1];
	}else{
		output[0] = cam.Position[0] - player.bowl->size[0]/2 - p.size[0];
		output[1] = cam.Position[1] + p.size[1]/2;
	}
	return output;
}

void Game::clearDeadEnemies(){
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i]->health <= 0){
			points += 2;
			enemyMultiplier += 0.1 + player.level/100.0;
			player.experience += 5;
			if(board_enemies[i]->name.compare("Melee") == 0)
				numEnemyKilled[0]++;
			delete board_enemies[i];
			board_enemies.erase(board_enemies.begin() + i);
			--i;
		}
	}
}

void Game::processEffectsForEnemies(const float dt){
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i]->effects.size() > 0){
			board_enemies[i]->applyEffects(dt);
		}
	}
}

void Game::processEffectsForPlayer(const float dt){
	player.applyEffects(dt);
}

void Game::processPlayerMovement(const float dt){
	if(dt > 0.5)
		return;
	if(player.velocity[1] < 0)
		player.falling = true;
	if(Keys[GLFW_KEY_W]){
		if(upCounter == 0)
			player.falling = true;
		//need to add code in the collision detector that will change falling to false
		if(upCounter < 0.5){
			upCounter += dt;
			player.velocity.y = 30 + maxSpeed/2 + player.speed/4;
		}
	}
	if(Keys[GLFW_KEY_S]){
		if(player.velocity.y > 0){
			player.velocity.y -= (player.speed + acceleration) * 2 * dt;
		}else
			player.velocity.y -= (player.speed + acceleration) * dt;
	}
	//move left, with correct acceleration
	if(Keys[GLFW_KEY_A]){
		player.facing = false;
		if(player.velocity.x > 0)
			player.velocity.x -= (player.speed + acceleration) * 2 * dt;
		else
			player.velocity.x -= (player.speed + acceleration) * dt;
		if(player.velocity.x < 0)
			player.velocity.x = std::max(player.velocity.x, static_cast<float>((-maxSpeed) - (player.speed/4)));
	}
	//move right, with correct acceleration
	if(Keys[GLFW_KEY_D]){
		player.facing = true;
		if(player.velocity.x < 0)
			player.velocity.x += (player.speed + acceleration) * 2 * dt;
		else
			player.velocity.x += (player.speed + acceleration) * dt;
		if(player.velocity.x > 0)
			player.velocity.x = std::min(player.velocity.x, static_cast<float>(maxSpeed + (player.speed/4)));
	}
	//slowing down, correct acceleration
	if(player.velocity.x != 0 && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D]){
		if(player.velocity.x < 0){
			if(player.velocity.x > (-(player.speed + acceleration + 50) - 0.2) * dt)
				player.velocity.x = 0;
			else
				player.velocity.x += ((player.speed + acceleration + 50) + 0.2) * dt;
		}else{
			if(player.velocity.x < (player.speed + acceleration + 50 + 0.2) * dt)
				player.velocity.x = 0;
			else
				player.velocity.x -= (player.speed + acceleration + 50 + 0.2) * dt;
		}
	}
	//always subtract 0.6 from the y velocity to simulate falling
	//makes it so that whenever they walk off a block they also fall
	//gets corrected to 0 in the collision detection if there is a collision
	//between the bottom of the player and the top of the object
	player.velocity.y -= acceleration * dt;
	player.velocity.y = std::max(player.velocity.y, (float)(-maxSpeed - 30 - player.speed/4));
		
	if(upCounter != 0 && !player.falling){
		upCounter = 0;
	}
}

void Game::moveAllProjectiles(const float dt){
	for(int i{}; i < player_projectiles.size(); ++i){
		player_projectiles[i].move(dt);
	}
	for(int i{}; i < enemy_projectiles.size(); ++i){
		enemy_projectiles[i].move(dt);
	}
}

void Game::processPlantInteraction(){
	board[player.location[0]][player.location[1]][player.location[2]].removePlant(player.location[3]);
	plantOffsets.clear();
	plantTexCoords.clear();
	numPlants = 0;
	for(int i{}; i < 9; ++i){
		for(int j{}; j < 100; ++j){
			calculatePlantOffsets(i, j);
		}
	}
	PlantRenderer->setOffset(&plantOffsets[0], numPlants);
	PlantRenderer->setTextureCoords(&plantTexCoords[0], numPlants);
}

void Game::enemyAttackLogic(){
	for(int i{}; i < board_enemies.size(); ++i){
		if(!(abs(board_enemies[i]->position[0] - cam.Position[0]) > 1000) && !(abs(board_enemies[i]->position[1] - cam.Position[1]) > 1000)){
			if(board_enemies[i]->attackFunc(cam.Position)){
				enemy_projectiles.push_back(ResourceManager::GetProjectile(board_enemies[i]->projectileName));
				auto it = enemy_projectiles.rbegin();
				glm::vec2 startPosition = board_enemies[i]->getProjectileStartPositionForEnemy(*it);
				short direction = board_enemies[i]->attackRight ? 1 : -1;
				it->setDirection(startPosition, direction, board_enemies[i]->projectileSpeed);
				it->right = board_enemies[i]->attackRight;
				it->damage = board_enemies[i]->attack;
			};
		}
	}
}

void Game::moveEnemies(float dt){
	return;
}
/*




RENDERING







*/
void Game::renderStartScreen(){
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("titlePage"), glm::vec2(-Width/2, -Height/2), glm::vec2(Width, Height));
}

void Game::renderHomeMain(){
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("homePage"), glm::vec2(-Width/2, -Height/2), glm::vec2(Width, Height));
}

void Game::renderArmoryScreen(){
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("exitSign"), glm::vec2(-Width/2, Height/2 - 40), glm::vec2(100, 40));
	auto it = ResourceManager::Bowls.begin();
	while(bowlCounter--){
		++it;
	}
	bowlCounter = (*it).second.second;
	if(bowls[bowlCounter])
		staticImageRenderer->DrawSprite((*it).second.first.attackAnimation[0],glm::vec2(0-(*it).second.first.size[0]/2, 0-(*it).second.first.size[1]/2),(*it).second.first.size);
	else
		staticImageRenderer->DrawSprite((*it).second.first.attackAnimation[0], glm::vec2(0-(*it).second.first.size[0]/2, 0-(*it).second.first.size[1]/2), (*it).second.first.size, 0.0, glm::vec3(0, 0, 0));
}

void Game::renderAchievements(){
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("exitSign"), glm::vec2(-Width/2, Height/2 - 40), glm::vec2(100, 40));
	
	glm::mat4 view = cam.GetViewMatrix();
	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&achievementOffsets[0], numAchievements);
	ProjectileRenderer->setTextureCoords(&achievementTexCoords[0], numAchievements);
	ProjectileRenderer->DrawSprites(numAchievements, ResourceManager::GetTexture("achievements"), maxAchievementSize, glm::vec2(-190.0, 110.0));

	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("highlight2"), glm::vec2(-190 + (achievementSelector%7) * 55, 110 - (achievementSelector/7) * 55), glm::vec2(45,45));
	
	if(viewingAchievement){
		staticImageRenderer->DrawSprite(ResourceManager::GetTexture(achievementNames[achievementSelector]), glm::vec2(-Width/2, -Height/2), glm::vec2(Width, Height));
	}
}

void Game::renderDeathScreen(){
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("deathPage"), glm::vec2(-Width/2, -Height/2), glm::vec2(Width, Height));
	glm::mat4 view = cam.GetViewMatrix();
	textRenderer->setViewMatrix("view", view);
	textRenderer->setOffset(&pointOffsets[0], pointTexCoords.size());
	textRenderer->setTextureCoords(&pointTexCoords[0], pointTexCoords.size());
	textRenderer->DrawSprites(pointTexCoords.size(), ResourceManager::GetTexture("numbers"), glm::vec2(20, 40), glm::vec2(deathScreenCoords[0] + 100.0,deathScreenCoords[1] - 100.0));
}

void Game::renderGreenhouse(){
	//675
	//693
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture(greenhouse[plantSelector].plantName), glm::vec2(-395, 45), glm::vec2(220, 220));
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture(greenhouse[plantSelector].boostName), glm::vec2(-170, 85), glm::vec2(220, 180));
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("emptyProgressBar"), glm::vec2(-150, 55), glm::vec2(200, 20));
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("progressBar"), glm::vec2(-150, 55), glm::vec2(200.0 * (greenhouse[plantSelector].experience / ((greenhouse[plantSelector].level + 1) * 1000.0f)), 20));
	staticImageRenderer->DrawSprite(ResourceManager::GetTexture("highlight2"), glm::vec2(-297.5 + (plantSelector%10)*60.0, -20 - (plantSelector/10)*60.0), glm::vec2(55, 55));

	glm::mat4 view = cam.GetViewMatrix();
	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&selectedPlantOffset[0], 6);
	ProjectileRenderer->setTextureCoords(&selectedPlantTexCoords[0], 6);
	ProjectileRenderer->DrawSprites(6, ResourceManager::GetTexture("plants"), 55.0, glm::vec2(137.5, 165));

	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&greenhouseOffsets[0], 50);
	ProjectileRenderer->setTextureCoords(&greenhouseTexCoords[0], 50);
	ProjectileRenderer->DrawSprites(50, ResourceManager::GetTexture("plants"), 55.0, glm::vec2(-297.5, -20));

	textRenderer->setViewMatrix("view", view);
	textRenderer->setOffset(&greenhouseLevelOffset[0], 1);
	textRenderer->setTextureCoords(&greenhouse[plantSelector].level, 1);
	textRenderer->DrawSprites(1, ResourceManager::GetTexture("numbers"), glm::vec2(20, 40), glm::vec2(-170, 45));
}

void Game::renderGame(){
	glm::mat4 view = cam.GetViewMatrix();
	renderGameBackground(view);
	//prepping data for the block rendering
	if(generatedChunks){
		calculateNewRenderValues();
	}
	calculateProjectileRenderValues();

	calculateEnemyRenderValues();

	calculateIconRenderValues();

	calculateTextRenderValues();

	if(player.currentPlant != -1){
		findHighlightPosition();
		findLevelIconPosition();
	}

	renderBlocks(view);
	renderPlants(view);
	renderEnemyProjectiles(view);
	renderPlayerProjectiles(view);
	renderEnemies(view);
	renderPlayer(view);
	renderUI(view);
	renderText(view);
}

void Game::renderGameBackground(glm::mat4 &view){
	BackGround_l1->setViewMatrix("view", view);
	BackGround_l1->setOffset(backgroundLayerOneOffset);
	BackGround_l1->DrawSprite(backgroundTextures->layerOne, glm::vec2(cam.Position[0], cam.Position[1]));

	BackGround_l2->setViewMatrix("view", view);
	BackGround_l2->setOffset(backgroundLayerTwoOffset);
	BackGround_l2->DrawSprite(backgroundTextures->layerTwo, glm::vec2(cam.Position[0], cam.Position[1]));

	BackGround_l3->setViewMatrix("view", view);
	BackGround_l3->setOffset(backgroundLayerThreeOffset);
	BackGround_l3->DrawSprite(backgroundTextures->layerThree, glm::vec2(cam.Position[0], cam.Position[1]));
}

void Game::renderBlocks(glm::mat4 &view){
	BlockRenderer->setViewMatrix("view", view);
	BlockRenderer->DrawInstancedSprites(numBlocks, ResourceManager::GetTexture("block"),
		glm::vec2(0.0f, -(blockSize - player.bowl->size[1])), glm::vec2((float)blockSize, (float)blockSize));
}

void Game::renderPlants(glm::mat4 &view){
	PlantRenderer->setViewMatrix("view", view);
	PlantRenderer->DrawSprites(numPlants, ResourceManager::GetTexture("plants"), maxPlantSize, glm::vec2(0.0f, -(maxPlantSize - player.bowl->size[1])));
}

void Game::renderEnemyProjectiles(glm::mat4 &view){
	EnemyProjectileRenderer->setViewMatrix("view", view);
	EnemyProjectileRenderer->setOffset(&enemyProjectileOffsets[0], enemy_projectiles.size());
	EnemyProjectileRenderer->setTextureCoords(&enemyProjectileTexCoords[0], enemy_projectiles.size());
	EnemyProjectileRenderer->DrawSprites(enemy_projectiles.size(), ResourceManager::GetTexture("enemyProjectiles"), maxEnemyProjectileSize, glm::vec2(0.0f, player.bowl->size[1] - maxEnemyProjectileSize));
}

void Game::renderPlayerProjectiles(glm::mat4 &view){
	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&playerProjectileOffsets[0], player_projectiles.size());
	ProjectileRenderer->setTextureCoords(&playerProjectileTexCoords[0], player_projectiles.size());
	ProjectileRenderer->DrawSprites(player_projectiles.size(), ResourceManager::GetTexture("projectiles"), maxProjectileSize, glm::vec2(0.0f, (player.bowl->size[1] - maxProjectileSize)));
}

void Game::renderEnemies(glm::mat4 &view){
	EnemyRenderer->setViewMatrix("view", view);
	EnemyRenderer->setOffset(&enemyOffsets[0], board_enemies.size());
	EnemyRenderer->setTextureCoords(&enemyTexCoords[0], board_enemies.size());
	EnemyRenderer->DrawEnemies(board_enemies.size(), ResourceManager::GetTexture("enemies"), maxEnemySize, glm::vec2(0.0f, -(maxEnemySize - player.bowl->size[1])), &hitData[0], board_enemies.size());
}

void Game::renderPlayer(glm::mat4 &view){
	Renderer->setViewMatrix("view", view);
	if(player.health == playerHealth && !player.isHit){
		Renderer->DrawSprite(player.bowl->attackAnimation[player.bowl->frameCounter], 
			glm::vec2(cam.Position[0] - player.bowl->size[0]/2, cam.Position[1] + player.bowl->size[1]/2),
			player.bowl->size);
	}else{
		if(player.isHit == 0)
			player.isHit = 0.100;
		Renderer->DrawSprite(player.bowl->attackAnimation[player.bowl->frameCounter], 
			glm::vec2(cam.Position[0] - player.bowl->size[0]/2, cam.Position[1] + player.bowl->size[1]/2),
			player.bowl->size, 0.0f, glm::vec3(0.5f, 0.0f, 0.0f));
		playerHealth = player.health;
	}
}

void Game::renderText(glm::mat4 &view){
	textRenderer->setViewMatrix("view", view);
	textRenderer->setOffset(&textOffsets[0], textTexCoords.size());
	textRenderer->setTextureCoords(&textTexCoords[0], textTexCoords.size());
	textRenderer->DrawSprites(textTexCoords.size(), ResourceManager::GetTexture("numbers"), glm::vec2(20, 40), glm::vec2(cam.Position[0] - 300, cam.Position[1] - 295));

	textRenderer->setViewMatrix("view", view);
	textRenderer->setOffset(&pointOffsets[0], pointTexCoords.size());
	textRenderer->setTextureCoords(&pointTexCoords[0], pointTexCoords.size());
	textRenderer->DrawSprites(pointTexCoords.size(), ResourceManager::GetTexture("numbers"), glm::vec2(20, 40), glm::vec2(cam.Position[0] - 80, cam.Position[1] - 295));
}

void Game::renderUI(glm::mat4 &view){
	UIRenderer->setViewMatrix("view", view);
	if(player.bowl->numOfPlants == 3){
		UIRenderer->DrawSprite(ResourceManager::GetTexture("ui_three_plant"), 
			glm::vec2(cam.Position[0] - Width/2, cam.Position[1] - Height/2),
			glm::vec2(Width, Height));
	}else{
		UIRenderer->DrawSprite(ResourceManager::GetTexture("ui_four_plant"), 
			glm::vec2(cam.Position[0] - Width/2, cam.Position[1] - Height/2),
			glm::vec2(Width, Height));
	}

	IconRenderer->setViewMatrix("view", view);
	IconRenderer->setOffset(&plantIconOffsets[0], numPlantIcon);
	IconRenderer->setTextureCoords(&plantIconTexCoords[0], numPlantIcon);
	IconRenderer->DrawSprites(numPlantIcon, ResourceManager::GetTexture("plants"), maxPlantIconSize, glm::vec2(cam.Position[0], cam.Position[1] - maxPlantIconSize));

	if(player.currentPlant != -1){
		Renderer->setViewMatrix("view", view);
		Renderer->DrawSprite(ResourceManager::GetTexture("highlight"), 
			highlightPosition, glm::vec2(35, 35));

		IconRenderer->setViewMatrix("view", view);
		IconRenderer->setOffset(&levelIconOffsets[0], player.numPlants);
		IconRenderer->setTextureCoords(&levelIconTexCoords[0], player.numPlants);
		IconRenderer->DrawSprites(player.numPlants, ResourceManager::GetTexture("levels"), maxLevelIconSize, glm::vec2(cam.Position[0], cam.Position[1] - maxLevelIconSize));
	}

	if(player.effects.size() > 0){
		IconRenderer->setViewMatrix("view", view);
		IconRenderer->setOffset(&effectIconOffsets[0], player.effects.size());
		IconRenderer->setTextureCoords(&effectIconTexCoords[0], player.effects.size());
		IconRenderer->DrawSprites(player.effects.size(), ResourceManager::GetTexture("effectIcons"), maxEffectIconSize, glm::vec2(cam.Position[0], cam.Position[1] - maxEffectIconSize));
	}
}

/*




CALCULATING RENDER VALUES




*/
void Game::calculateNewRenderValues(){
	//clear already existing offset data
	blockOffsets.clear();
	plantOffsets.clear();
	plantTexCoords.clear();
	numBlocks = 0;
	numPlants = 0;
	//only need to recalculate every time that we generate new chunks
	for(short i{}; i < 9; ++i){
		for(short j{}; j < 100; ++j){
			calculateBlockOffsets(i, j);
			calculatePlantOffsets(i, j);
		}
	}
	generatedChunks = false;
	BlockRenderer->bindInstanceBuffer(&blockOffsets[0], numBlocks);
	PlantRenderer->setOffset(&plantOffsets[0], numPlants);
	PlantRenderer->setTextureCoords(&plantTexCoords[0], numPlants);
}

void Game::calculateBlockOffsets(const short i, const short j){
	for(short k{}; k < board[i/3][i%3][j].objects.size(); ++k){
		glm::vec2 temp{};
		//calculating the x offset
		//the object is going to be normally rendered at 0,0 with
		//bottom right corner at 50, -50
		//setting offset initially for chunks themselves
		switch(i % 3){
			case 0 :
				temp[0] = -edgeDistace;
				break;
			case 1 :
				temp[0] = -innerRegionEdgeDistance;
				break;
			case 2 :
				temp[0] = innerRegionEdgeDistance;
				break;
		}
		//then add for each little box in a chunk
		temp[0] += (j % 10) * chunkSize;
		//then add per block offset
		temp[0] += board[i/3][i%3][j].objects[k]->position[0];

		//calculating the y offset
		//set offset initially for chunks themselves
		switch(i / 3){
			case 0 :
				temp[1] = edgeDistace;
				break;
			case 1 :
				temp[1] = innerRegionEdgeDistance;
				break;
			case 2 :
				temp[1] = -innerRegionEdgeDistance;
				break;
		}
		//subtracting for each little box in a chunk
		temp[1] -= (j / 10) * chunkSize;
		//adding position per block which is already negative
		temp[1] += board[i/3][i%3][j].objects[k]->position[1];

		temp[0] /= blockSize;
		temp[1] /= blockSize;

		blockOffsets.push_back(temp);

		numBlocks++;
	}
}

void Game::calculatePlantOffsets(const short i, const short j){
	for(short k{}; k < board[i/3][i%3][j].plants.size(); ++k){
		glm::vec2 temp{};
		//calculating the x offset
		//setting offset initially for chunks themselves
		switch(i % 3){
			case 0 :
				temp[0] = -edgeDistace;
				break;
			case 1 :
				temp[0] = -innerRegionEdgeDistance;
				break;
			case 2 :
				temp[0] = innerRegionEdgeDistance;
				break;
		}
		//then add for each little box in a chunk
		temp[0] += (j % 10) * chunkSize;
		//then add the plants position
		temp[0] += board[i/3][i%3][j].plants[k].position[0];

		//calculating the y offset
		//set offset initially for chunks themselves
		switch(i / 3){
			case 0 :
				temp[1] = edgeDistace;
				break;
			case 1 :
				temp[1] = innerRegionEdgeDistance;
				break;
			case 2 :
				temp[1] = -innerRegionEdgeDistance;
				break;
		}
		//subtracting for each little box in a chunk
		temp[1] -= (j / 10) * chunkSize;
		temp[1] += board[i/3][i%3][j].plants[k].position[1];

		temp[0] /= maxPlantSize;
		temp[1] /= maxPlantSize;

		plantOffsets.push_back(temp);

		numPlants++;

		plantTexCoords.push_back(ResourceManager::getDepth(board[i/3][i%3][j].plants[k].textureName));
	}
}

void Game::calculateProjectileRenderValues(){
	enemyProjectileOffsets.clear();
	enemyProjectileTexCoords.clear();
	playerProjectileOffsets.clear();
	playerProjectileTexCoords.clear();
	for(int i{}; i < enemy_projectiles.size(); ++i){
		enemyProjectileOffsets.push_back(glm::vec2(enemy_projectiles[i].position[0]/maxEnemyProjectileSize, enemy_projectiles[i].position[1]/maxEnemyProjectileSize));
		if(enemy_projectiles[i].right)
			enemyProjectileTexCoords.push_back(ResourceManager::getDepth(enemy_projectiles[i].name + ""));
		else
			enemyProjectileTexCoords.push_back(ResourceManager::getDepth(enemy_projectiles[i].name + "reverse"));
	}

	for(int i{}; i < player_projectiles.size(); ++i){
		playerProjectileOffsets.push_back(glm::vec2(player_projectiles[i].position[0]/maxProjectileSize, player_projectiles[i].position[1]/maxProjectileSize));
		if(player_projectiles[i].right)
			playerProjectileTexCoords.push_back(ResourceManager::getDepth(player_projectiles[i].name + ""));
		else
			playerProjectileTexCoords.push_back(ResourceManager::getDepth(player_projectiles[i].name + "reverse"));
	}
}

void Game::calculateEnemyRenderValues(){
	enemyOffsets.clear();
	enemyTexCoords.clear();
	hitData.clear();
	for(int i{}; i < board_enemies.size(); ++i){
		enemyOffsets.push_back(glm::vec2(board_enemies[i]->position[0]/maxEnemySize, board_enemies[i]->position[1]/maxEnemySize));
		enemyTexCoords.push_back(ResourceManager::getDepth(board_enemies[i]->name));
		hitData.push_back(board_enemies[i]->hitData);
	}
}

void Game::calculateIconRenderValues(){
	plantIconOffsets.clear();
	plantIconTexCoords.clear();
	effectIconOffsets.clear();
	effectIconTexCoords.clear();
	numEffectIcon = player.effects.size();
	for(int i{}; i < player.effects.size(); ++i){

	}
	numPlantIcon = player.numPlants;
	//the first square for 3 plant ui goes from
	//445 to 480
	//top side is at 750
	short starting_offset = player.bowl->numOfPlants == 3 ? threePlantFirstBoxX : fourPlantFirstBoxX;
	for(int i{}; i < player.numPlants; ++i){
		//no idea where the 45 came from but it works so i'm keeping it lol
		plantIconOffsets.push_back(glm::vec2((starting_offset + ((plantBoxSize + plantBoxSpacing) * i) + (45.0-maxPlantIconSize)/2)/maxPlantIconSize, (plantFirstBoxY - (45.0-maxPlantIconSize)/2)/maxPlantIconSize));
		plantIconTexCoords.push_back(ResourceManager::getDepth(player.plants[i].name));
	}

	starting_offset = 50;
	for(int i{}; i < player.effects.size(); ++i){
		effectIconOffsets.push_back(glm::vec2((starting_offset + (maxEffectIconSize + effectIconSpacing) * i)/maxEffectIconSize, plantFirstBoxY - (maxEffectIconSize + effectIconSpacing)*(i/8)));
		effectIconTexCoords.push_back(ResourceManager::getDepth(player.effects[i].name));
	}
}

void Game::moveBackground(float dt){
	backgroundLayerOneOffset[0] += ((player.velocity[0]/12) * dt)/backgroundSize;
	backgroundLayerOneOffset[1] += ((player.velocity[1]/12) * dt)/backgroundSize;
	backgroundLayerTwoOffset[0] += ((player.velocity[0]/8) * dt)/backgroundSize;
	backgroundLayerTwoOffset[1] += ((player.velocity[1]/8) * dt)/backgroundSize;
	backgroundLayerThreeOffset[0] += ((player.velocity[0]/5) * dt)/backgroundSize;
	backgroundLayerThreeOffset[1] += ((player.velocity[1]/5) * dt)/backgroundSize;
}

void Game::findHighlightPosition(){
	if(player.numPlants >= 1){
		if(player.bowl->numOfPlants == 3){
			highlightPosition = glm::vec2(threePlantFirstBoxX + ((plantBoxSize + plantBoxSpacing) * player.currentPlant), plantFirstBoxY);
		}else{
			highlightPosition = glm::vec2(fourPlantFirstBoxX + ((plantBoxSize + plantBoxSpacing) * player.currentPlant), plantFirstBoxY);
		}
		highlightPosition[0] += cam.Position[0];
		highlightPosition[1] += cam.Position[1] - plantBoxSize;
	}
}

void Game::findLevelIconPosition(){
	levelIconOffsets.clear();
	levelIconTexCoords.clear();
	for(int i{}; i < player.numPlants; ++i){
		levelIconTexCoords.push_back(player.plants[i].level - 1);
		if(player.bowl->numOfPlants == 3)
			levelIconOffsets.push_back(glm::vec2((threePlantFirstBoxX + ((plantBoxSize + plantBoxSpacing) * i) + levelBarSpacing)/maxLevelIconSize, (plantFirstBoxY - levelBarSpacing)/maxLevelIconSize));
		else
			levelIconOffsets.push_back(glm::vec2((fourPlantFirstBoxX + ((plantBoxSize + plantBoxSpacing) * i) + levelBarSpacing)/maxLevelIconSize, (plantFirstBoxY - levelBarSpacing)/maxLevelIconSize));
	}
}

void Game::calculateTextRenderValues(){
	textTexCoords.clear();
	textOffsets.clear();
	pointOffsets.clear();
	pointTexCoords.clear();
	int healthNumber = player.health;
	std::vector<short> tempNums{};
	while(healthNumber > 0){
		tempNums.push_back(healthNumber % 10);
		healthNumber /= 10;
	}
	auto it = tempNums.rbegin();
	for(int i{}; i < tempNums.size(); ++i){
		textTexCoords.push_back(*(it + i));
		textOffsets.push_back(glm::vec2((22 * i)/20.0, 0));
	}

	int tempPoints = static_cast<int>(points);
	tempNums.clear();
	if(tempPoints == 0){
		tempNums.push_back(0);
	}else{
		while(tempPoints > 0){
			tempNums.push_back(tempPoints%10);
			tempPoints/=10;
		}
	}

	it = tempNums.rbegin();
	for(int i{}; i < tempNums.size(); ++i){
		pointTexCoords.push_back(*(it + i));
		pointOffsets.push_back(glm::vec2((22 * i)/20.0, 0));
	}
}
/*






SHADER && RENDERER LOADING






*/
void Game::initShaders(){
	ResourceManager::LoadShader("bin/shaders/vertexShader.txt", "bin/shaders/fragShader.txt", nullptr, "player");
	ResourceManager::LoadShader("bin/shaders/block_vshader.txt", "bin/shaders/fragShader.txt", nullptr, "block");
	ResourceManager::LoadShader("bin/shaders/texSamp_vshader.txt", "bin/shaders/fragShader_array.txt", nullptr, "plant");
	ResourceManager::LoadShader("bin/shaders/texSamp_vshader.txt", "bin/shaders/fragShader_array.txt", nullptr, "projectiles");
	ResourceManager::LoadShader("bin/shaders/texSamp_vshader.txt", "bin/shaders/fragShader_array.txt", nullptr, "enemyProjectiles");
	ResourceManager::LoadShader("bin/shaders/enemy_vshader.txt", "bin/shaders/fragShader_enemy.txt", nullptr, "enemy");
	ResourceManager::LoadShader("bin/shaders/background_vshader.txt", "bin/shaders/fragShader.txt", nullptr, "background_l1");
	ResourceManager::LoadShader("bin/shaders/background_vshader.txt", "bin/shaders/fragShader.txt", nullptr, "background_l2");
	ResourceManager::LoadShader("bin/shaders/background_vshader.txt", "bin/shaders/fragShader.txt", nullptr, "background_l3");
	ResourceManager::LoadShader("bin/shaders/texSamp_vshader.txt", "bin/shaders/fragShader_array.txt", nullptr, "text");
	ResourceManager::LoadShader("bin/shaders/vertexShader.txt", "bin/shaders/fragShader.txt", nullptr, "UI");
	ResourceManager::LoadShader("bin/shaders/texSamp_vshader.txt", "bin/shaders/fragShader_array.txt", nullptr, "icon");
	ResourceManager::LoadShader("bin/shaders/static_vshader.txt", "bin/shaders/fragShader.txt", nullptr, "staticImage");
}

void Game::initRenderers(){
	//configure shaders
	glm::mat4 projection = glm::ortho(-Width/2, Width/2,
		-Height/2, Height/2, -1.0f, 1.0f);
	glm::mat4 view = cam.GetViewMatrix();
	initRenderer(view, projection);
	initBlockRenderer(view, projection);
	initPlantRenderer(view, projection);
	initProjectileRenderer(view, projection);
	initEnemyRenderer(view, projection);
	initBackgroundRenderers(projection);
	initTextRenderer(view, projection);
	initUIRenderer(view, projection);
	initIconRenderer(view, projection);
	initEnemyProjectileRenderer(view, projection);
	initStaticImageRenderer(projection);
}

void Game::initRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("player");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	Renderer = new SpriteRenderer(ResourceManager::GetShader("player"));
}

void Game::initBlockRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("block");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	BlockRenderer = new SpriteRenderer(ResourceManager::GetShader("block"));
}

void Game::initPlantRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("plant");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	PlantRenderer = new TexSampRenderer(ResourceManager::GetShader("plant"));
}

void Game::initProjectileRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("projectiles");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	ProjectileRenderer = new TexSampRenderer(ResourceManager::GetShader("projectiles"));
}

void Game::initEnemyRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("enemy");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	EnemyRenderer = new TexSampRenderer(ResourceManager::GetShader("enemy"));
}

void Game::initBackgroundRenderers(glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("background_l1");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	BackGround_l1 = new BackgroundRenderer(ResourceManager::GetShader("background_l1"));

	sProgram = ResourceManager::GetShader("background_l2");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	BackGround_l2 = new BackgroundRenderer(ResourceManager::GetShader("background_l2"));

	sProgram = ResourceManager::GetShader("background_l3");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	BackGround_l3 = new BackgroundRenderer(ResourceManager::GetShader("background_l3"));
}

void Game::initTextRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("text");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	textRenderer = new TexSampRenderer(ResourceManager::GetShader("text"));
}

void Game::initUIRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("UI");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	UIRenderer = new SpriteRenderer(ResourceManager::GetShader("UI"));
}

void Game::initIconRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("icon");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	IconRenderer = new TexSampRenderer(ResourceManager::GetShader("icon"));
}

void Game::initEnemyProjectileRenderer(glm::mat4 &view, glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("enemyProjectiles");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	EnemyProjectileRenderer = new TexSampRenderer(ResourceManager::GetShader("enemyProjectiles"));
}

void Game::initStaticImageRenderer(glm::mat4 &projection){
	Shader sProgram;
	sProgram = ResourceManager::GetShader("staticImage");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	staticImageRenderer = new SpriteRenderer(ResourceManager::GetShader("staticImage"));
}