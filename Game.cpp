#include "Game.h"
#include "GameObject.h"

SpriteRenderer *Renderer;
SpriteRenderer *BlockRenderer;
SpriteRenderer *textRenderer;
//background renderer
BackgroundRenderer *BackGround_l1;
BackgroundRenderer *BackGround_l2;
BackgroundRenderer *BackGround_l3;
//all block, plant, and projectile textures need to have the
//same space or else I can't load the model matrix
TexSampRenderer *PlantRenderer;
TexSampRenderer *ProjectileRenderer;
TexSampRenderer *EnemyRenderer;

Game::Game(unsigned int width, unsigned int height){
	m_state = HOME_MAIN;
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
}

void Game::Init(){
	initShaders();
	initRenderers();
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
	if(m_state != START_SCREEN && m_state != DEATH_SCREEN){
		processPlayerMovement(dt);
		if(m_state == HOME_MAIN){
			if(Keys[GLFW_KEY_I]){
				// if(player.interact){
				// 	if(player.interact->sprite.ID == ResourceManager::GetTexture("gate").ID){
				// 		m_state = GAME_ACTIVE_CLASSIC;
				// 		initializeGame();
				// 		cam.Position[0] = 0;
				// 		cam.Position[1] = 0;
				// 		points = 0;
				// 		//should make a portal class
				// 		//need to have some function in there that can be called for
				// 		//all interactables
				// 		//like interact with or something
				// 	}else if(player.interact->sprite.ID == ResourceManager::GetTexture("bossgate").ID){
				// 		m_state = GAME_ACTIVE_BOSS;
				// 		//add in code here to prep for that
				// 	}else if(player.interact->sprite.ID == ResourceManager::GetTexture("armory").ID){
				// 		m_state = HOME_ARMORY;
				// 		//add in code later
				// 	}else if(player.interact->sprite.ID == ResourceManager::GetTexture("nursery").ID){
				// 		m_state = HOME_NURSERY;
				// 		//add in code later
				// 	}else if(player.interact->sprite.ID == ResourceManager::GetTexture("greenhouse").ID){
				// 		m_state = HOME_GREENHOUSE;
				// 		//add code later
				// 	}else if(player.interact->sprite.ID == ResourceManager::GetTexture("clock").ID){
				// 		m_state = HOME_CLOCK;
				// 	}
				// }
				m_state = GAME_ACTIVE_CLASSIC;
				initializeGame();
				player.setStatBoosts();
				cam.Position[0] = 0;
				cam.Position[1] = 0;
				points = 0;
			}
		}else if(m_state == GAME_ACTIVE_CLASSIC){
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
					std::cout << "registered that there was something to interact with\n";
					player.interact->interact(this);
					if(player.interact->type.compare("plant") == 0){
						std::cout << "about to delete plant\n";
						board[player.location[0]][player.location[1]][player.location[2]].removePlant(player.location[3]);
						std::cout << "recalculating the values\n";
						for(int i{}; i < 9; ++i){
							for(int j{}; j < 100; ++j){
								calculatePlantOffsets(i, j);
							}
						}
						PlantRenderer->setOffset(&plantOffsets[0], numPlants);
						PlantRenderer->setTextureCoords(&plantTexCoords[0], numPlants);
					}
					points += 5;
					player.experience += 2;
				}
			}
		}
	}else if(m_state == START_SCREEN){
		if(Keys[GLFW_KEY_SPACE]){
			m_state = HOME_MAIN;
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

	if(m_state == HOME_MAIN){
		//don't know what I'm doing here, need to set it to the chunk that is displayed
		Chunk *home_main = &ResourceManager::GetChunk("home_main");

		for(int i{}; i < home_main->objects.size(); ++i){
			player_and_object_collisions(home_main->objects[i], dt);
		}
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		if(abs(player.velocity.x) >= 0.01 || abs(player.velocity.y) >= 0.01){
			points += dt/2;
			player.experience += dt/20;
		}
		//moving the projectiles
		moveAllProjectiles(dt);

		//testing if it is time to generate new chunks
		//0 up
		//1 right
		//2 down
		//3 left
		if(cam.Position[0] >= 4500){
			generateChunks(1);
			fixPlayerPosition(1);
		}else if(cam.Position[0] <= -4500){
			generateChunks(3);
			fixPlayerPosition(3);
		}

		if(cam.Position[1] >= 4500){
			generateChunks(0);
			fixPlayerPosition(0);
		}else if(cam.Position[1] <= -4500){
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
		backgroundLayerOneOffset[0] += ((player.velocity[0]/12) * dt)/backgroundSize;
		backgroundLayerOneOffset[1] += ((player.velocity[1]/12) * dt)/backgroundSize;
		backgroundLayerTwoOffset[0] += ((player.velocity[0]/8) * dt)/backgroundSize;
		backgroundLayerTwoOffset[1] += ((player.velocity[1]/8) * dt)/backgroundSize;
		backgroundLayerThreeOffset[0] += ((player.velocity[0]/5) * dt)/backgroundSize;
		backgroundLayerThreeOffset[1] += ((player.velocity[1]/5) * dt)/backgroundSize;
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
						if(rnum <= 5){
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
							end->position.x += ((j-10)%10) * 50;
							end->position.y -= ((j-10)/10) * 50;
						}else if(rnum <= 30){
							std::uniform_int_distribution enemyPicker{0, static_cast<int>(numOfEnemies) - 1};
							//20% chance to spawn an enemy
							//stored inside of a vector for the Game class
							//might change this to have a certain percentage per enemy
							//and then spawn that enemy in
							int enemyNum = enemyPicker(mersenne);
							auto it = ResourceManager::Enemies.begin();
							while(enemyNum--)
								++it;
							board_enemies.push_back((it)->second);
							fixGeneratedEnemiesPosition(i, j, generationCodes2[k], generationCode[k]);
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
	ResourceManager::Enemies["Melee"] = Melee();
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

	std::cout << "player died?\n";
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
						if(rnum <= 5){
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
							end->position.x += ((j-10)%10) * 50;
							end->position.y -= (((j-10)/10)) * 50;
						}else if(rnum <= 30){
							std::uniform_int_distribution enemyPicker{0, static_cast<int>(numOfEnemies) - 1};
							//20% chance to spawn an enemy
							//stored inside of a vector for the Game class
							//might change this to have a certain percentage per enemy
							//and then spawn that enemy in
							int enemyNum = enemyPicker(mersenne);
							auto it = ResourceManager::Enemies.begin();
							while(enemyNum--)
								++it;
							board_enemies.push_back((it)->second);
							fixGeneratedEnemiesPosition(i, j, k, direction);
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
			min_y = -7500;
			max_y = -2500;
			min_x = -7500;
			max_x = 7500;
			break;
		case 1 :
			min_y = -7500;
			max_y = 7500;
			min_x = -7500;
			max_x = -2500;
			break;
		case 2 :
			min_y = 2500;
			max_y = 7500;
			min_x = -7500;
			max_x = 7500;
			break;
		case 3 :
			min_y = -7500;
			max_y = 7500;
			min_x = 2500;
			max_x = 7500;
			break;
	}
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i].position[0] >= min_x && board_enemies[i].position[0] <= max_x 
			&& board_enemies[i].position[1] >= min_y && board_enemies[i].position[1] <= max_y){
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
			min_y = -7500;
			max_y = -2500;
			min_x = -7500;
			max_x = 7500;
			break;
		case 1 :
			min_y = -7500;
			max_y = 7500;
			min_x = -7500;
			max_x = -2500;
			break;
		case 2 :
			min_y = 2500;
			max_y = 7500;
			min_x = -7500;
			max_x = 7500;
			break;
		case 3 :
			min_y = -7500;
			max_y = 7500;
			min_x = 2500;
			max_x = 7500;
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
	auto end = board_enemies.rbegin();
	//i is the mini chunk inside the larger chunk
	//i/10 gives the row
	//i%10 gives the column
	//
	//j is the location of the block inside the mini chunk
	//j/10 gives the row
	//j%10 gives the column
	//
	//k is the chunk being generated
	end->position.x = ((i%10)-5)*500 + (((j-10)%10) * 50) + ((50-maxEnemySize)/2);
	end->position.y = (5-(i/10))*500 - (((j-10)/10) * 50) - (50-maxEnemySize);

	switch(direction){
		case 0 :
			end->position.y += 5000;
			if(k == 0)
				end->position.x -= 5000;
			else if(k == 2)
				end->position.x += 5000;
			break;
		case 1 :
			end->position.x += 5000;
			if(k == 0)
				end->position.y += 5000;
			else if(k == 2)
				end->position.y -= 5000;
			break;
		case 2 :
			end->position.y -= 5000;
			if(k == 0)
				end->position.x -= 5000;
			else if(k == 2)
				end->position.x += 5000;
			break;
		case 3 :
			end->position.x -= 5000;
			if(k == 0)
				end->position.y += 5000;
			else if(k == 2)
				end->position.y -= 5000;
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
			board_enemies[i].position[1] += adder;
		else
			board_enemies[i].position[0] += adder;
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
			return 5000;
		case 0 :
		case 1 :
			return -5000;
	}
	return 0;
}

void Game::fixPlayerPosition(const short direction){
	switch(direction){
		case 0:
			cam.Position[1] -= 5000;
			break;
		case 1:
			cam.Position[0] -= 5000;
			break;
		case 2:
			cam.Position[1] += 5000;
			break;
		case 3:
			cam.Position[0] += 5000;
			break;
	}
}
/*




COLLISION DETECTION




*/
void Game::player_and_object_collisions(GameObject *object, const float dt, const short gameobject_offset_x, const short gameobject_offset_y){
	if(object->interactable){
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
			&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] - player.bowl->size[1]/2 <= object->position[1] + gameobject_offset_y
			&& object->position[1] - object->size[1] + gameobject_offset_y <= cam.Position[1] + player.bowl->size[1]/2;
		if(collisionX && collisionY){
			std::cout << "on interactable object\n";
			//set the object as the interactable object
			player.interact = object;
			player.location = findInteractPosition(object, gameobject_offset_x, gameobject_offset_y);
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

void Game::findLocationCoordinates(short &width, short &height, const float x, const float y){
	//this changes the variables to the index
	//no returns, just changes the variables passed to it
	if(y / 500 == 0){
		if(y < 0){
			height = -1;
		}else{
			height = 0;
		}
	}else{
		if(y < 0){
			height = y/500 -1;
		}else{
			height = y/ 500;
		}
	}
	if(x / 500 == 0){
		if(x < 0){
			width = -1;
		}else{
			width = 0;
		}
	}else{
		if(x < 0){
			width = x/500 -1;
		}else{
			width = x/ 500;
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
			short gameobject_offset_y = (height + 1 + i) * 500;
			short gameobject_offset_x = (width + chunk_width_offset) * 500;
			for(int j{}; j < board[index_x][index_y][index_chunk + index_chunk_2].objects.size(); ++j){
				//loop through all of the blocks in the chunk
				player_and_object_collisions(board[index_x][index_y][index_chunk + index_chunk_2].objects[j], dt, gameobject_offset_x, gameobject_offset_y);
			}
			for(int j{}; j < board[index_x][index_y][index_chunk + index_chunk_2].plants.size(); ++j){
				//loop through all of the plants in the chunk
				player_and_object_collisions((GameObject *)&(board[index_x][index_y][index_chunk + index_chunk_2].plants[j]), dt, gameobject_offset_x, gameobject_offset_y);
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
			short gameobject_offset_y = (height + 1 + i) * 500;
			short gameobject_offset_x = (width + chunk_width_offset) * 500;
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
			if(abs(board_enemies[j].position[0] - player_projectiles[i].position[0]) > 500)
				continue;
			else if(abs(board_enemies[j].position[1] - player_projectiles[i].position[1]) > 500)
				continue;
			else
				if(game_classic_two_object_collisions((GameObject *)(&(board_enemies[j])), (GameObject *)&(player_projectiles[i]))){
					//deal damage
					board_enemies[j].health -= (player_projectiles[i].damage - board_enemies[j].defense);
					//add effects
					board_enemies[j].addEffects(player_projectiles[i]);
					if(!player_projectiles[i].piercing){
						player_projectiles.erase(player_projectiles.begin() + i);
						--i;
						deletionTracker = true;
						break;
					}
				}
		}
		//collision detection between the projectile and the edge
		if(!deletionTracker){
			if(abs(player_projectiles[i].position[0]) > 7500 || abs(player_projectiles[i].position[1]) > 7500){
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
			if(game_classic_two_object_collisions((GameObject *)&(player), (GameObject *)&(enemy_projectiles[i]))){
				player.dealDamage(enemy_projectiles[i].damage);
				enemy_projectiles.erase(enemy_projectiles.begin() + i);
				--i;
				deletionTracker = true;
				player.addEffects(enemy_projectiles[i]);
			}
		//collision detection between the projectile and the edge
		if(!deletionTracker){
			if(abs(enemy_projectiles[i].position[0]) > 7500 || abs(enemy_projectiles[i].position[1]) > 7500){
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

glm::vec4 Game::findInteractPosition(GameObject *object, short gameobject_offset_x, short gameobject_offset_y){
	short width, height, index_chunk;
	glm::vec4 output = {0.0f, 0.0f, 0.0f, 0.0f};
	findLocationCoordinates(width, height, gameobject_offset_x, gameobject_offset_y);

	if(width <= -6){
		output[0] = 0;
		output[2] = width + 15;
	}else if(width <= 4){
		output[0] = 1;
		output[2] = width + 5;
	}else{
		output[0] = 2;
		output[2] = width - 5;
	}

	if(height <= -6){
		output[1] = 2;
		output[2] += (-6 - (height)) * 10;
	}else if(height <= 4){
		output[1] = 1;
		output[2] += (4 - (height)) * 10;
	}else{
		output[1] = 0;
		output[2] += (14 - (height)) * 10;
	}

	for(auto it = board[output[0]][output[1]][output[2]].plants.begin(); it != board[output[0]][output[1]][output[2]].plants.end(); ++it){
		std::cout << it->position[0] << ',' << it->position[1] << '\n';
		std::cout << object->position[0] << ',' << object->position[1] << '\n';

		if(it->position == object->position){
			output[3] = it - board[output[0]][output[1]][output[2]].begin();
			break;
		}
	}
	std::cout << output[3] << '\n';
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
	}else{
		player_projectiles.push_back(ResourceManager::GetProjectile(player.plants[player.currentPlant].projectileName[player.plants[player.currentPlant].level]));
		auto it = player_projectiles.rbegin();
		glm::vec2 startPosition = getProjectileStartPositionForPlayer(*it);
		short direction = player.facing ? 1 : -1;
		it->setDirection(startPosition, direction);
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
		if(board_enemies[i].health <= 0){
			points += 2;
			player.experience += 5;
			board_enemies.erase(board_enemies.begin() + i);
			--i;
		}
	}
}

void Game::processEffectsForEnemies(const float dt){
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i].effects.size() > 0){
			board_enemies[i].applyEffects(dt);
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
			player.velocity.y = 30 + maxSpeed + player.speed/4;
		}
	}
	if(Keys[GLFW_KEY_S]){
		player.switchPlant();
	}
	//move left, with correct acceleration
	if(Keys[GLFW_KEY_A]){
		player.facing = false;
		player.velocity.x -= (player.speed + acceleration) * dt;
		if(player.velocity.x < 0)
			player.velocity.x = std::max(player.velocity.x, static_cast<float>((-maxSpeed) - (player.speed/4)));
	}
	//move right, with correct acceleration
	if(Keys[GLFW_KEY_D]){
		player.facing = true;
		player.velocity.x += (player.speed + acceleration) * dt;
		if(player.velocity.x > 0)
			player.velocity.x = std::min(player.velocity.x, static_cast<float>(maxSpeed + (player.speed/4)));
	}
	//slowing down, correct acceleration
	if(player.velocity.x != 0 && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D]){
		if(player.velocity.x < 0){
			if(player.velocity.x > (-(player.speed + acceleration) - 0.2) * dt)
				player.velocity.x = 0;
			else
				player.velocity.x += ((player.speed + acceleration) + 0.2) * dt;
		}else{
			if(player.velocity.x < (player.speed + acceleration + 0.2) * dt)
				player.velocity.x = 0;
			else
				player.velocity.x -= (player.speed + acceleration + 0.2) * dt;
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
/*




RENDERING







*/
void Game::renderHomeMain(){
	glm::mat4 view = cam.GetViewMatrix();
	Renderer->setViewMatrix("view", view);
	Chunk *home_main = &ResourceManager::GetChunk("home_main");
	for(int i{}; i < home_main->objects.size(); ++i){
		float tempSize = std::max(home_main->objects[i]->size[0], home_main->objects[i]->size[1]);
		Renderer->DrawSprite(home_main->objects[i]->sprite, 
			home_main->objects[i]->position,
			glm::vec2(tempSize, tempSize));
	}
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

	renderBlocks(view);
	renderPlants(view);
	renderEnemyProjectiles(view);
	renderPlayerProjectiles(view);
	renderEnemies(view);
	renderPlayer(view);
	//renderText();
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
		glm::vec2(0.0f, -(50 - player.bowl->size[1])), glm::vec2(50.0f, 50.0f));
}

void Game::renderPlants(glm::mat4 &view){
	PlantRenderer->setViewMatrix("view", view);
	PlantRenderer->DrawSprites(numPlants, ResourceManager::GetTexture("plants"), maxPlantSize, glm::vec2(0.0f, -(maxPlantSize - player.bowl->size[1])));
}

void Game::renderEnemyProjectiles(glm::mat4 &view){
	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&enemyProjectileOffsets[0], enemy_projectiles.size());
	ProjectileRenderer->setTextureCoords(&enemyProjectileTexCoords[0], enemy_projectiles.size());
	ProjectileRenderer->DrawSprites(enemy_projectiles.size(), ResourceManager::GetTexture("projectiles"), maxProjectileSize);
}

void Game::renderPlayerProjectiles(glm::mat4 &view){
	ProjectileRenderer->setViewMatrix("view", view);
	ProjectileRenderer->setOffset(&playerProjectileOffsets[0], player_projectiles.size());
	ProjectileRenderer->setTextureCoords(&playerProjectileTexCoords[0], player_projectiles.size());
	ProjectileRenderer->DrawSprites(player_projectiles.size(), ResourceManager::GetTexture("projectiles"), maxProjectileSize, glm::vec2(0.0f, player.bowl->size[1] - maxProjectileSize));
}

void Game::renderEnemies(glm::mat4 &view){
	EnemyRenderer->setViewMatrix("view", view);
	EnemyRenderer->setOffset(&enemyOffsets[0], board_enemies.size());
	EnemyRenderer->setTextureCoords(&enemyTexCoords[0], board_enemies.size());
	EnemyRenderer->DrawSprites(board_enemies.size(), ResourceManager::GetTexture("enemies"), maxEnemySize, glm::vec2(0.0f, -(maxEnemySize - player.bowl->size[1])));
}

void Game::renderPlayer(glm::mat4 &view){
	Renderer->setViewMatrix("view", view);
	Renderer->DrawSprite(player.bowl->attackAnimation[player.bowl->frameCounter], 
		glm::vec2(cam.Position[0] - player.bowl->size[0]/2, cam.Position[1] + player.bowl->size[1]/2),
		player.bowl->size);
}

void Game::renderText(){
	for(int i{}; i < text.size(); ++i){
		float tempSize = std::max(text[i]->size[0], text[i]->size[1]);
		textRenderer->DrawSprite(text[i]->sprite, text[i]->position, glm::vec2(tempSize, tempSize));
	}
	// implement health/plant rendering once I know what the game screen looks like
	// also need to implement point system
	// todo also need to implement effect rendering
	// Texture *hp = ResourceManager::GetTexture(to_string(player.health + player.getHealthBoost()));
	// tempSize = std::max(hp->width, hp->height);
}

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
				temp[0] = -7500;
				break;
			case 1 :
				temp[0] = -2500;
				break;
			case 2 :
				temp[0] = 2500;
				break;
		}
		//then add for each little box in a chunk
		temp[0] += (j % 10) * 500;
		//then add per block offset
		temp[0] += board[i/3][i%3][j].objects[k]->position[0];

		//calculating the y offset
		//set offset initially for chunks themselves
		switch(i / 3){
			case 0 :
				temp[1] = 7500;
				break;
			case 1 :
				temp[1] = 2500;
				break;
			case 2 :
				temp[1] = -2500;
				break;
		}
		//subtracting for each little box in a chunk
		temp[1] -= (j / 10) * 500;
		//adding position per block which is already negative
		temp[1] += board[i/3][i%3][j].objects[k]->position[1];

		temp[0] /= 50;
		temp[1] /= 50;

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
				temp[0] = -7500;
				break;
			case 1 :
				temp[0] = -2500;
				break;
			case 2 :
				temp[0] = 2500;
				break;
		}
		//then add for each little box in a chunk
		temp[0] += (j % 10) * 500;
		//then add the plants position
		temp[0] += board[i/3][i%3][j].plants[k].position[0];

		//calculating the y offset
		//set offset initially for chunks themselves
		switch(i / 3){
			case 0 :
				temp[1] = 7500;
				break;
			case 1 :
				temp[1] = 2500;
				break;
			case 2 :
				temp[1] = -2500;
				break;
		}
		//subtracting for each little box in a chunk
		temp[1] -= (j / 10) * 500;
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
		enemyProjectileOffsets.push_back(glm::vec2(enemy_projectiles[i].position[0]/maxProjectileSize, enemy_projectiles[i].position[1]/maxProjectileSize));
		enemyProjectileTexCoords.push_back(ResourceManager::getDepth(enemy_projectiles[i].name));
	}

	for(int i{}; i < player_projectiles.size(); ++i){
		playerProjectileOffsets.push_back(glm::vec2(player_projectiles[i].position[0]/maxProjectileSize, player_projectiles[i].position[1]/maxProjectileSize));
		playerProjectileTexCoords.push_back(ResourceManager::getDepth(player_projectiles[i].name));
	}
}

void Game::calculateEnemyRenderValues(){
	enemyOffsets.clear();
	enemyTexCoords.clear();
	for(int i{}; i < board_enemies.size(); ++i){
		enemyOffsets.push_back(glm::vec2(board_enemies[i].position[0]/maxEnemySize, board_enemies[i].position[1]/maxEnemySize));
		enemyTexCoords.push_back(ResourceManager::getDepth(board_enemies[i].name));
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
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader_array.txt", nullptr, "enemy");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l1");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l2");
	ResourceManager::LoadShader("shaders/background_vshader.txt", "shaders/fragShader.txt", nullptr, "background_l3");
	ResourceManager::LoadShader("shaders/text_vshader.txt", "shaders/fragShader.txt", nullptr, "text");
}

void Game::initRenderers(){
	//configure shaders
	glm::mat4 projection = glm::ortho(-300.0f, 300.0f,
		-400.0f , 400.0f, -1.0f, 1.0f);
	glm::mat4 view = cam.GetViewMatrix();
	initRenderer(view, projection);
	initBlockRenderer(view, projection);
	initPlantRenderer(view, projection);
	initProjectileRenderer(view, projection);
	initEnemyRenderer(view, projection);
	initBackgroundRenderers(projection);
	initTextRenderer(view, projection);
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
	textRenderer = new SpriteRenderer(ResourceManager::GetShader("text"));
}