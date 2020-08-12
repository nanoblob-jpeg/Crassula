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

Game::~Game(){
	delete Renderer;
	delete BlockRenderer;
	delete textRenderer;
	delete BackGround_l1;
	delete BackGround_l2;
	delete BackGround_l3;
	delete PlantRenderer;
	delete ProjectileRenderer;
	delete EnemyRenderer;

	std::ofstream file;
	file.open("bin/player.txt");
	file << player.name << '\n';
	file << std::to_string(player.level) << '\n';
	file << backgroundTextures->name << '\n';
	file << std::to_string(player.experience) << '\n';
	file << player.bowl->name;
	file.close();
}

void Game::Init(){
	initShaders();
	initRenderers();
	setUnlockedBowls();
	//load textures
	ResourceManager::LoadTexture2("textureDirectory.txt");
	ResourceManager::LoadArrayTextures("arrayTextureDirectory.txt");
	//load gameobject
	ResourceManager::LoadGameObject("gameObjectDirectory.txt");
	//load chunk
	ResourceManager::LoadChunk("home_main.txt", false);
	//loading chunks for the 10x10 squares
	ResourceManager::LoadChunk("chunk_list.txt", true);
	//load effects
	ResourceManager::LoadEffect("effect_list.txt");
	//load bowls
	ResourceManager::LoadBowl("bowl_list.txt");
	//load plants
	ResourceManager::LoadPlant("plant_list.txt");
	//loads backgrounds
	ResourceManager::LoadBackgrounds("background_list.txt");
	//load player
	player.loadPlayer("bin/player.txt");
	//load projectiles
	ResourceManager::LoadProjectiles("projectileDirectory.txt");
	//load enemies
	loadEnemies();
	setBackground(player.backgroundName);
	//create board
	prepBoard();
	reserveArraySpace();
	bowlCounter = ResourceManager::GetBowl(player.bowl->name).second;
}

void Game::ProcessInput(float dt){
	//this b key section is just for debugging and displaying when I need to
	if(Keys[GLFW_KEY_B]){
		std::cout << cam.Position[0] << ',' << cam.Position[1] << std::endl;
		for(int i{}; i < player_projectiles.size(); ++i){
			std::cout << player_projectiles[i].position[0] << ',' << player_projectiles[i].position[1] << std::endl;
			std::cout << player_projectiles[i].size[0] << ',' << player_projectiles[i].size[1] << std::endl;
		}
		std::cout << std::endl;
	}
	if(m_state != START_SCREEN && m_state != DEATH_SCREEN && m_state != HOME_ARMORY && m_state != HOME_MAIN){
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
					enemyMultiplier += 0.2;
					points += 5;
					player.experience += 2;
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
			m_state = GAME_ACTIVE_CLASSIC;
			initializeGame();
			player.setStatBoosts();
			player.setFinalStats();
			playerHealth = player.health;
			cam.Position[0] = 0;
			cam.Position[1] = 0;
			points = 0;
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
	}else if(m_state == HOME_ACHIEVEMENTS){
		if(Keys[GLFW_KEY_W] && !achievementMoved){
			if(achievementSelector <= 14){
				achievementSelector += 120;
			}else{
				achievementSelector -= 14;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_A] && !achievementMoved){
			if(achievementSelector%14 == 0){
				achievementSelector += 13;
			}else{
				achievementSelector -= 1;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_D] && !achievementMoved){
			if((achievementSelector-13)%14 == 0){
				achievementSelector -= 13;
			}else{
				achievementSelector += 1;
			}
			achievementMoved = true;
		}
		if(Keys[GLFW_KEY_S] && !achievementMoved){
			if(achievementSelector >= 126){
				achievementSelector -= 126;
			}else{
				achievementSelector += 14;
			}
			achievementMoved = true;
		}
		if(!Keys[GLFW_KEY_W] && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_S] && !Keys[GLFW_KEY_D]){
			achievementMoved = false;
		}
		if(Keys[GLFW_KEY_I]){
			viewingAchievement = true;
		}
	}
};

void Game::Update(float dt){
	//todo once I add achievements, I need to add the checks here


	//move the player
	cam.ProcessKeyboard(player.velocity, dt);
	player.interact = nullptr;
	if(player.bowl->inAnimation){
		player.bowl->addFrameTimer(dt);
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
			enemyMultiplier += 0.001 * dt;
			player.experience += dt/20;
		}
		//moving the projectiles
		moveAllProjectiles(dt);

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
		//check if the player has died
		if(player.isDead()){
			gameEndProtocol();
		}
		enemyAttackLogic();
		moveBackground(dt);
	}

	//test for the other states else if()
};

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

	if(m_state == HOME_MAIN){
		renderHomeMain();
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		renderGame();
	}else if(m_state == START_SCREEN){
		renderStartScreen();
	}else if(m_state == HOME_ARMORY){
		renderArmoryScreen();
	}
}
/*



GAME INITIALIZATION/DESTRUCTION





*/
//this is to make the fix enemy location function work as intended
const std::vector<short> generationCode{0, 0, 0, 3, 4, 1, 2, 2, 2};
const std::vector<short> generationCodes2{0, 1, 2, 1, 4, 1, 0, 1, 2};
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

void Game::loadEnemies(){
	//create the basic enemy here except don't have position set
	//then save it to the map as a broad class enemy not
	//as the specific one
	//then they select it and set the position
	ResourceManager::setDepth("Melee", 0);
}

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

void Game::setBackground(std::string &name){
	backgroundTextures = &ResourceManager::GetBackground(name);
}

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

void Game::reserveArraySpace(){
	blockOffsets.reserve(30000);
	plantOffsets.reserve(30000);
	plantTexCoords.reserve(30000);
	enemyProjectileOffsets.reserve(100);
	enemyProjectileTexCoords.reserve(600);
	playerProjectileOffsets.reserve(100);
	playerProjectileTexCoords.reserve(600);
	enemyOffsets.reserve(30000);
	enemyTexCoords.reserve(30000);
}

void Game::gameEndProtocol(){
	clearAndResetGameBoard();
			
	player.calculateLevel();
	player.calculateStats();
	player.interact = nullptr;
	player.plants.clear();
	player.effects.clear();
	player.velocity[0] = 0;
	player.velocity[1] = 0;

	board_enemies.clear();
	enemy_projectiles.clear();
	player_projectiles.clear();
	player.statBoosts.clear();

	m_state = DEATH_SCREEN;

	cam.Position[0] = 0;
	cam.Position[1] = 0;

	enemyMultiplier = 1.0;

	std::cout << "player died?\n";
}

void Game::setUnlockedBowls(){
	std::string line;
	std::ifstream fstream("bin/bowlUnlockFile.txt");
	if(fstream){
		while(std::getline(fstream, line)){
			bowls.push_back(line == "1" ? true : false);
			std::getline(fstream, line);
			bowlNames.push_back(line);
		}
	}else{
		std::cout << "unlock bowl file not fount\n";
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
					if(!player_projectiles[i].piercing || !board_enemies[j]->hitByPiercing)
						board_enemies[j]->health -= std::max(player_projectiles[i].damage - board_enemies[j]->defense, 1);
					//add effects
					board_enemies[j]->addEffects(player_projectiles[i]);
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
			enemyMultiplier += 0.1;
			player.experience += 5;
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
	//renderText(view);
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
	EnemyRenderer->DrawSprites(board_enemies.size(), ResourceManager::GetTexture("enemies"), maxEnemySize, glm::vec2(0.0f, -(maxEnemySize - player.bowl->size[1])));
}

void Game::renderPlayer(glm::mat4 &view){
	std::cout << player.health << '\n' << playerHealth << std::endl;
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
	textRenderer->setOffset(&textOffsets[0], textOffsets.size());
	textRenderer->setTextureCoords(&textTexCoords[0], textTexCoords.size());
	//change this render command
	textRenderer->DrawSprites(textOffsets.size(), ResourceManager::GetTexture("alphabet"), maxTextHeight, glm::vec2(cam.Position[0], cam.Position[1] - maxTextHeight));
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
	for(int i{}; i < board_enemies.size(); ++i){
		enemyOffsets.push_back(glm::vec2(board_enemies[i]->position[0]/maxEnemySize, board_enemies[i]->position[1]/maxEnemySize));
		enemyTexCoords.push_back(ResourceManager::getDepth(board_enemies[i]->name));
	}
}

void Game::calculateIconRenderValues(){
	effectsIconOffsets.clear();
	effectsIconTexCoords.clear();
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
	setNeededText();
	for(int i{}; i < text.size(); ++i){
		int newLine = 0;
		for(int j{}; j < text[i].first.length(); ++j){
			if(text[i].first[j] == '\n'){
				newLine++;
				continue;
			}
			textTexCoords.push_back(ResourceManager::getDepth(std::string(1, text[i].first[j])));
			textOffsets.push_back(glm::vec2((text[i].second[0] + maxTextWidth * j)/maxTextWidth, (text[i].second[1] + (maxTextHeight * newLine))));
		}
	}
}

void Game::setNeededText(){
	text.clear();
	if(m_state == GAME_ACTIVE_CLASSIC){
		text.push_back(std::pair<std::string, glm::vec2>(std::to_string(player.health), glm::vec2(562 - 300, 90 - 400)));
		text.push_back(std::pair<std::string, glm::vec2>(std::to_string(points), glm::vec2(562 - 300, 136 - 400)));
	}
}
/*






SHADER && RENDERER LOADING






*/
void Game::initShaders(){
	ResourceManager::LoadShader("shaders/vertexShader.txt", "shaders/fragShader.txt", nullptr, "player");
	ResourceManager::LoadShader("shaders/block_vshader.txt", "shaders/fragShader.txt", nullptr, "block");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "plant");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "projectiles");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "enemyProjectiles");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "enemy");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l1");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l2");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l3");
	ResourceManager::LoadShader("shaders/text_vshader.txt", "shaders/fragShader.txt", nullptr, "text");
	ResourceManager::LoadShader("shaders/vertexShader.txt", "shaders/fragShader.txt", nullptr, "UI");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "icon");
	ResourceManager::LoadShader("shaders/static_vshader.txt", "shaders/fragShader.txt", nullptr, "staticImage");
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