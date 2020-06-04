#include "Game.h"

SpriteRenderer *Renderer;
SpriteRenderer *BlockRenderer;
TexSampRenderer *PlantRenderer;

void Game::Init(){
	//load shaders
	ResourceManager::LoadShader("shaders/vertexShader.txt", "shaders/fragShader.txt", nullptr, "player");
	ResourceManager::LoadShader("shaders/block_vshader.txt", "shaders/fragShader.txt", nullptr, "block");
	ResourceManager::LoadShader("shaders/texSamp_vshader.txt", "shaders/fragShader.txt", nullptr, "plant");
	//configure shaders
	glm::mat4 projection = glm::ortho(-300.0f, 300.0f,
		-400.0f , 400.0f, -1.0f, 1.0f);
	glm::mat4 view = cam.GetViewMatrix();
	Shader sProgram;
	sProgram = ResourceManager::GetShader("player");
	sProgram.use();
	sProgram.setInt("image", 0);
	sProgram.setMat4("projection", projection);
	sProgram.setMat4("view", view);
	Renderer = new SpriteRenderer(ResourceManager::GetShader("player"));


	Shader bProgram;
	bProgram = ResourceManager::GetShader("block");
	bProgram.use();
	bProgram.setInt("image", 0);
	bProgram.setMat4("projection", projection);
	bProgram.setMat4("view", view);
	BlockRenderer = new SpriteRenderer(ResourceManager::GetShader("block"));

	Shader cProgram;
	cProgram = ResourceManager::GetShader("plant");
	cProgram.use();
	cProgram.setInt("image", 0);
	cProgram.setMat4("projection", projection);
	cProgram.setMat4("view", view);
	PlantRenderer = new TexSampRenderer(ResourceManager::GetShader("plant"));
	/*
	load textures
	 */
	//loads all in one function call
	ResourceManager::LoadTexture2("textureDirectory.txt");

	/*
	load GameObjects
	the building blocks for the chunks
	*/
	ResourceManager::LoadGameObject("gameObjectDirectory.txt");

	/*
	load chunks
	 */
	//loading home_main chunk
	ResourceManager::LoadChunk("home_main.txt", false);
	//loading chunks for the 10x10 squares
	ResourceManager::LoadChunk("chunk_list.txt", true);

	/*
	load effects
	*/
	ResourceManager::LoadEffect("effect_list.txt");

	/*
	load bowls
	*/
	ResourceManager::LoadBowl("bowl_list.txt");
	
	/*
	load plants
	*/
	ResourceManager::LoadPlant("plant_list.txt");

	/*
	load player
	 */
	player.loadPlayer("bin/player.txt");

	/*
	load the enemies
	can't be loaded from a file as each of them have different attack functions
	*/
	loadEnemies();

	//create board
	for(int i{}; i < 3; ++i){
		std::deque<std::vector<Chunk>> temp;
		for(int j{}; j < 3; ++j){
			std::vector<Chunk> temp2(100);
			temp.push_back(temp2);
		}
		board.push_back(temp);
	}

	blockOffsets.reserve(30000);
	plantOffsets.reserve(30000);
	plantTexCoords.reserve(30000);
}

Game::~Game(){
	delete Renderer;
}

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
		Renderer->DrawSprite(ResourceManager::GetTexture("face"),
			glm::vec2(200.0f, 200.0f), glm::vec2(50.0f, 50.0f),
				45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		//prepping data for the block rendering
		if(generatedChunks){
			//clear already existing offset data
			blockOffsets.clear();
			plantOffsets.clear();
			//only need to recalculate every time that we generate new chunks
			for(int i{}; i < 9; ++i){
				for(int j{}; j < 100; ++j){
					for(int k{}; k < board[i/3][i%3][j].objects.size(); ++k){
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

						blockOffsets.push_back(temp);

						numBlocks++;
					}
					for(int k{}; k < board[i/3][i%3][j].plants.size(); ++k){
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
						temp[1] -= board[i/3][i%3][j].plants[k].position[1];

						plantOffsets.push_back(temp);

						numPlants++;

						for(int l{}; l < 6; l++){
							plantTexCoords.push_back(board[i/3][i%3][j].plants[k].texturePosition[l]);
						}
					}
				}
			}
			generatedChunks = false;
			BlockRenderer->bindInstanceBuffer(&blockOffsets[0], numBlocks);
			PlantRenderer->setOffset(&plantOffsets[0], numPlants);
			PlantRenderer->setTextureCoords(&plantTexCoords[0], numPlants);
		}
		glm::mat4 view = cam.GetViewMatrix();
		BlockRenderer->setViewMatrix("view", view);
		//don't forget to update the view matrix every time this loop runs
		BlockRenderer->DrawInstancedSprites(numBlocks, ResourceManager::GetTexture("block"),
			glm::vec2(0.0f, 0.0f), glm::vec2(50.0f, 50.0f));

		PlantRenderer->DrawSprites(numPlants, ResourceManager::GetTexture("plants"));
		
		float tempSize = std::max(player.bowl->size[0], player.bowl->size[1]);
		Renderer->DrawSprite(player.bowl->attackAnimation[player.bowl->frameCounter], 
			glm::vec2(cam.Position[0] - player.bowl->size[0], cam.Position[1] + player.bowl->size[1]),
			glm::vec2(tempSize, tempSize));
	}
	
}

void Game::Update(float dt){
	//todo once I add achievements, I need to add the checks here


	//move the player
	cam.ProcessKeyboard(player.velocity, dt);
	player.interact = nullptr;

	if(m_state == HOME_MAIN){
		//don't know what I'm doing here, need to set it to the chunk that is displayed
		Chunk *home_main = &ResourceManager::GetChunk("home_main");

		for(int i{}; i < home_main->objects.size(); ++i){
			player_and_object_collisions(home_main->objects[i], dt);
		}
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		//moving the projectiles
		moveAllProjectiles(dt);

		//testing if it is time to generate new chunks
		//0 up
		//1 right
		//2 down
		//3 left
		if(player.position[0] >= 4500)
			generateChunks(1);
		else if(player.position[0] <= -4500)
			generateChunks(3);

		if(player.position[1] >= 4500)
			generateChunks(0);
		else if(player.position[1] <= -4500)
			generateChunks(2);

		//finding out which chunk the player is in using position
		int width, height;
		//function returns which square the player is in
		findLocationCoordinates(width, height, cam.Position[0], cam.Position[1]);

		//collision detection between player and objects(the ground/plants/gates)
		//since we have the player exact location, we only check the 9
		//chunks that are right around it
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
			clearAndResetGameBoard();
			
			player.calculateStats();
			player.interact = nullptr;
			for(int i{}; i < player.plants.size(); ++i){
				player.plants[i] = nullptr;
			}
			player.effects.clear();
			player.position[0] = 0;
			player.position[1] = 0;
			player.velocity[0] = 0;
			player.velocity[1] = 0;

			m_state = DEATH_SCREEN;
		}
	}


	//test for the other states else if()
};

void Game::ProcessInput(float dt){
	if(m_state != START_SCREEN && m_state != DEATH_SCREEN){
		if(Keys[GLFW_KEY_W]){
			if(upCounter == 0)
				player.falling = true;
			//need to add code in the collision detector that will change falling to false
			if(upCounter < 0.5){
				upCounter += dt;
				player.velocity.y = 4.1;
			}
		}
		if(Keys[GLFW_KEY_S]){
			player.switchPlant();
		}
		//move left, with correct acceleration
		if(Keys[GLFW_KEY_A]){
			player.velocity.x -= player.speed;
			if(player.velocity.x < 0)
				player.velocity.x = std::max(player.velocity.x, static_cast<float>((-3.5) - (player.speed/4)));
		}
		//move right, with correct acceleration
		if(Keys[GLFW_KEY_D]){
			player.velocity.x += player.speed;
			if(player.velocity.x > 0)
				player.velocity.x = std::min(player.velocity.x, static_cast<float>(3.5 + (player.speed/4)));
		}
		//slowing down, correct acceleration
		if(player.velocity.x != 0 && !Keys[GLFW_KEY_A] && !Keys[GLFW_KEY_D]){
			if(player.velocity.x < 0){
				if(player.velocity.x > -player.speed - 0.2)
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
		//always subtract 0.6 from the y velocity to simulate falling
		//makes it so that whenever they walk off a block they also fall
		//gets corrected to 0 in the collision detection if there is a collision
		//between the bottom of the player and the top of the object
		player.velocity.y -= 0.6;
		
		if(upCounter != 0 && !player.falling){
			upCounter = 0;
		}

		if(m_state == HOME_MAIN){
			if(Keys[GLFW_KEY_I]){
				if(player.interact->sprite.ID == ResourceManager::GetTexture("gate").ID){
					m_state = GAME_ACTIVE_CLASSIC;
					initializeGame();

					//should make a portal class
					//need to have some function in there that can be called for
					//all interactables
					//like interact with or something
				}else if(player.interact->sprite.ID == ResourceManager::GetTexture("bossgate").ID){
					m_state = GAME_ACTIVE_BOSS;
					//add in code here to prep for that
				}else if(player.interact->sprite.ID == ResourceManager::GetTexture("armory").ID){
					m_state = HOME_ARMORY;
					//add in code later
				}else if(player.interact->sprite.ID == ResourceManager::GetTexture("nursery").ID){
					m_state = HOME_NURSERY;
					//add in code later
				}else if(player.interact->sprite.ID == ResourceManager::GetTexture("greenhouse").ID){
					m_state = HOME_GREENHOUSE;
					//add code later
				}else if(player.interact->sprite.ID == ResourceManager::GetTexture("clock").ID){
					m_state = HOME_CLOCK;
				}
			}
		}else if(m_state == GAME_ACTIVE_CLASSIC){

		}
	}else if(m_state = START_SCREEN){
		if(Keys[GLFW_KEY_SPACE]){
			m_state = HOME_MAIN;
		}
	}
};

	//this is to make the fix enemy location function work as intended
	const std::vector<short> generationCode{0, 0, 0, 3, 4, 1, 2, 2, 2};
void Game::initializeGame(){
	generatedChunks = true;
	std::uniform_int_distribution chunkSelector{1,static_cast<int>(numOfChunks)};
	std::uniform_int_distribution random{1, 100};
	for(int k{}; k < 9; ++k){
		std::vector<Chunk> temp;
		//getting all the chunks
		for(int i{}; i < 100; ++i){
			temp.push_back(ResourceManager::GetChunk(std::to_string(chunkSelector(mersenne))));
		}
		//add spawning logic here
		for(int i{}; i < 100; ++i){
			for(int j{10}; j < 100; ++j){
				if(temp[i].locationOfObjects[j]){
					if(!temp[i].locationOfObjects[j-10]){
						int rnum = random(mersenne);
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
							end->position.x = ((j-10)/10) * 50 + (50 - (it)->second.size[0])/2;
							end->position.y = ((j-10)%10) * 50 + (50 - (it)->second.size[1]);
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
							fixGeneratedEnemiesPosition(i, j, k%3, generationCode[k]);
						}
					}
				}
			}
		}
		board[k/3][k%3] = temp;
	}
}

void Game::generateChunks(int direction){
	generatedChunks = true;
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
	delete &temp;
	despawnEnemiesFromDeletedChunks(direction);
	fixRemainingEnemyPosition(direction);
	despawnProjectilesFromDeletedChunks(direction);
	fixRemainingProjectilePosition(direction);

	std::uniform_int_distribution chunkSelector{1,static_cast<int>(numOfChunks)};
	std::uniform_int_distribution random{1, 100};

	for(int k{}; k < 3; ++k){
		std::vector<Chunk> temp;
		//getting all the chunks
		for(int i{}; i < 100; ++i){
			temp.push_back(ResourceManager::GetChunk(std::to_string(chunkSelector(mersenne))));
		}
		//add spawning logic here
		for(int i{}; i < 100; ++i){
			for(int j{10}; j < 100; ++j){
				if(temp[i].locationOfObjects[j]){
					if(!temp[i].locationOfObjects[j-10]){
						int rnum = random(mersenne);
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
							end->position.x = ((j-10)%10) * 50 + (50 - (it)->second.size[0])/2;
							end->position.y = ((j-10)/10) * 50 + (50 - (it)->second.size[1])/2;
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

void Game::fixGeneratedEnemiesPosition(int i, int j, int k, int direction){
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
	end->position.x = ((i%10)-5)*500 + ((j%10) * 50);
	end->position.y = (4-(i/10))*500 - ((j/10) * 50);

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

void Game::loadEnemies(){
	//create the basic enemy here except don't have position set
	//then save it to the map as a broad class enemy not
	//as the specific one
	//then they select it and set the position
}

void Game::player_and_object_collisions(GameObject *object, float dt, int gameobject_offset_x, int gameobject_offset_y){
	if(object->interactable){
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
			&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= object->position[1] + gameobject_offset_y
			&& object->position[1] + object->size[1] + gameobject_offset_y >= cam.Position[1] - player.bowl->size[1]/2;
		if(collisionX && collisionY){
			//set the object as the interactable object
			player.interact = object;
		}
	}else{
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= object->position[0] + gameobject_offset_x
			&& object->position[0] + object->size[0] + gameobject_offset_x >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= object->position[1] + gameobject_offset_y
			&& object->position[1] + object->size[1] + gameobject_offset_y >= cam.Position[1] - player.bowl->size[1]/2;

		if(collisionX && collisionY){
			//getting previous location
			short direction = findPlayerDirection(object, dt, gameobject_offset_x, gameobject_offset_y);
			
			//applying the corrections to the players position
			//while also fixing the velocity in that direction
			//to make it seem like they were stopped by the object
			if(direction == 0){
				cam.Position[1] -= cam.Position[1] + player.bowl->size[1]/2 - (object->position[1] + gameobject_offset_y);
				player.velocity[1] = 0.0f;
				player.falling = false;
			}else if(direction == 1){
				cam.Position[0] += object->position[0] + object->size[0] + gameobject_offset_x - (cam.Position[0] - player.bowl->size[0]/2);
				player.velocity[0] = 0.0f;
			}else if(direction == 2){
				cam.Position[1] += object->position[1] + object->size[1] + gameobject_offset_y - (cam.Position[1] - player.bowl->size[1]/2);
				player.velocity[1] = 0.0f;
			}else{
				cam.Position[0] -= cam.Position[0] + player.bowl->size[0]/2 - (object->position[0] + gameobject_offset_x);
				player.velocity[0] = 0.0f;
			}
		}
	}
}

short Game::findPlayerDirection(GameObject *object, float dt, int gameobject_offset_x, int gameobject_offset_y){
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
	if(prevPosition[1] + player.bowl->size[1]/2 < object->position[1] + gameobject_offset_y){
		//testing to see if the player with clearly above the block
		if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) &&  prevPosition[0] - (object->position[0] + gameobject_offset_x) < object->size[0] + gameobject_offset_x + player.bowl->size[0]/2){
			direction = 0;
		}else{
			//testing the edge cases where the player is in the corner of the space around the block
			//yet still collides with it this frame
			//uses distance for each axis and the velocity in order to figure out which one
			//hit first
			float distanceX = prevPosition[0] < object->position[0] + gameobject_offset_x ? (object->position[0] + gameobject_offset_x) - (prevPosition[0] + player.bowl->size[0]/2) 
				: object->position[0] + object->size[0] + gameobject_offset_x - (prevPosition[0] - player.bowl->size[0]/2);
			float distanceY = (object->position[1] + gameobject_offset_y) - (prevPosition[1] + player.bowl->size[1]/2);

			if(distanceX/player.velocity[0] < distanceY/(-player.velocity[1])){
				if(prevPosition[0] < object->position[0] + gameobject_offset_x){
					direction = 3;
				}else{
					direction = 1;
				}
			}else{
				direction = 0;
			}
		}
	}

	//testing when the player is below the object
	else if(prevPosition[1] - player.bowl->size[1]/2 > object->position[1] + object->size[1] + gameobject_offset_y){
		//testing to see if the player with clearly above the block
		if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) && prevPosition[0] - (object->position[0] + gameobject_offset_x) < object->size[0] + player.bowl->size[0]/2){
			direction = 2;
		}else{
			//testing the edge cases where the player is in the corner of the space around the block
			//yet still collides with it this frame
			//uses distance for each axis and the velocity in order to figure out which one
			//hit first
			float distanceX = prevPosition[0] < object->position[0] + gameobject_offset_x ? (object->position[0] + gameobject_offset_x) - (prevPosition[0] + player.bowl->size[0]/2) 
				: object->position[0] + object->size[0] + gameobject_offset_x - (prevPosition[0] - player.bowl->size[0]/2);
			float distanceY = prevPosition[1] - player.bowl->size[1]/2 - (object->position[1] + gameobject_offset_y);

			if(distanceX/player.velocity[0] < distanceY/(player.velocity[1])){
				if(prevPosition[0] < object->position[0]){
					direction = 3;
				}else{
					direction = 1;
				}
			}else{
				direction = 2;
			}
		}
	}

	//testing when the rest of the situations where the block approaches from the side
	//previous ifs have eliminated the top and bottom approaches
	else if(prevPosition[0] < object->position[0] + gameobject_offset_x){
		direction = 3;
	}else{
		direction = 1;
	}
	return direction;
}

bool Game::game_classic_two_object_collisions(GameObject *object, GameObject *projectile){
	bool collisionX =  projectile->position[0] + projectile->size[0] >= object->position[0]
		&& object->position[0] + object->size[0] >= projectile->position[0];
	bool collisionY = projectile->position[1] + projectile->size[1] >= object->position[1]
		&& object->position[1] + object->size[1] >= projectile->position[1];
	return (collisionX && collisionY);
}

bool Game::game_classic_two_object_collisions(GameObject *object, GameObject *object2, int gameobject_offset_x, int gameobject_offset_y){
	bool collisionX =  object2->position[0] + object2->size[0] >= object->position[0] + gameobject_offset_x
		&& object->position[0] + object->size[0] + gameobject_offset_x >= object2->position[0];
	bool collisionY = object2->position[1] + object2->size[1] >= object->position[1] + gameobject_offset_y
		&& object->position[1] + object->size[1] + gameobject_offset_y >= object->position[1];
	return (collisionX && collisionY);
}

void Game::findLocationCoordinates(int &width, int &height, float x, float y){
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

void Game::nineBlockCollisionDetectionPAndO(int width, int height, float dt){
	//finding the chunks right next to the main chunk
	short chunk_width_offset = width == -15 ? 0 : -1;
	short max_width_chunk_offset = width == 14 ? 0 : 1;
	short chunk_height_offset = height == -15 ? 0 : -1;
	short max_height_chunk_offset = height == 14 ? 0 : 1;
	//iterating through those chunks
	for(; chunk_width_offset <= max_width_chunk_offset; chunk_width_offset++){
		//figuring out which chunk on the board it corresponds to
		short index_x, index_y, index_chunk;
		if(width <= -6){
			index_y = 0;
			index_chunk += width + chunk_width_offset + 15;
		}else if(width <= 4){
			index_y = 1;
			index_chunk += width + chunk_width_offset + 5;
		}else{
			index_y = 2;
			index_chunk += width + chunk_width_offset - 5;
		}
		for(int i = chunk_height_offset; i <= max_height_chunk_offset; i++){
			if(height <= -6){
				index_x = 2;
				index_chunk += (-6 - height + i) * 10;
			}else if(height <= 4){
				index_x = 1;
				index_chunk += (4 - height + i) * 10;
			}else{
				index_x = 0;
				index_chunk += (14 - height + i) * 10;
			}

			//check this chunk and loop through the gameobjects
			//detect collisions between them and the player object
			//the height offset needed for each chunk is equal to the (y coordinate + 1) of the location multiplied by 500
			//the width offset needed for each chunk is equal to the x coordinate of the location multiplied by 500
			int gameobject_offset_y = (height + 1 + i) * 500;
			int gameobject_offset_x = (width + chunk_width_offset) * 500;
			for(int j{}; j < board[index_x][index_y][index_chunk].objects.size(); ++j){
				//loop through all of the blocks in the chunk
				player_and_object_collisions(board[index_x][index_y][index_chunk].objects[j], gameobject_offset_x, gameobject_offset_y, dt);
			}
			for(int j{}; j < board[index_x][index_y][index_chunk].plants.size(); ++j){
				//loop through all of the plants in the chunk
				player_and_object_collisions((GameObject *)&(board[index_x][index_y][index_chunk].plants[j]), gameobject_offset_x, gameobject_offset_y, dt);
			}
		}
	}
}

bool Game::nineBlockCollisionDetectionGeneral(int width, int height, GameObject *object){
	//finding the chunks right next to the main chunk
	short chunk_width_offset = width == -15 ? 0 : -1;
	short max_width_chunk_offset = width == 14 ? 0 : 1;
	short chunk_height_offset = height == -15 ? 0 : -1;
	short max_height_chunk_offset = height == 14 ? 0 : 1;
	//iterating through those chunks
	for(; chunk_width_offset <= max_width_chunk_offset; chunk_width_offset++){
		//figuring out which chunk on the board it corresponds to
		short index_x, index_y, index_chunk;
		if(width <= -6){
			index_y = 0;
			index_chunk += width + chunk_width_offset + 15;
		}else if(width <= 4){
			index_y = 1;
			index_chunk += width + chunk_width_offset + 5;
		}else{
			index_y = 2;
			index_chunk += width + chunk_width_offset - 5;
		}
		for(int i = chunk_height_offset; i <= max_height_chunk_offset; i++){
			if(height <= -6){
				index_x = 2;
				index_chunk += (-6 - height + i) * 10;
			}else if(height <= 4){
				index_x = 1;
				index_chunk += (4 - height + i) * 10;
			}else{
				index_x = 0;
				index_chunk += (14 - height + i) * 10;
			}

			//check this chunk and loop through the gameobjects
			//detect collisions between them and the player object
			//the height offset needed for each chunk is equal to the (y coordinate + 1) of the location multiplied by 500
			//the width offset needed for each chunk is equal to the x coordinate of the location multiplied by 500
			int gameobject_offset_y = (height + 1 + i) * 500;
			int gameobject_offset_x = (width + chunk_width_offset) * 500;
			for(int j{}; j < board[index_x][index_y][index_chunk].objects.size(); ++j){
				//loop through all of the blocks in the chunk
				if(game_classic_two_object_collisions(board[index_x][index_y][index_chunk].objects[j], object, gameobject_offset_x, gameobject_offset_y))
					return true;
			}
		}
	}
	return false;
}

void Game::player_projectile_collision_detection(){
	int width, height;
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
					for(auto k = player_projectiles[i].effects->begin(); k != player_projectiles[i].effects->end(); ++k){
						board_enemies[j].effects.push_back(*k);
					}
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
			}
		}
	}
}

void Game::enemy_projectile_collision_detection(){
	int width, height;
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
				for(auto k = enemy_projectiles[i].effects->begin(); k != enemy_projectiles[i].effects->end(); ++k){
					player.effects.push_back(*k);
				}
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
	}
}

void Game::clearDeadEnemies(){
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i].health <= 0){
			board_enemies.erase(board_enemies.begin() + i);
			--i;
		}
	}
}

void Game::processEffectsForEnemies(float dt){
	for(int i{}; i < board_enemies.size(); ++i){
		if(board_enemies[i].effects.size() > 0){
			board_enemies[i].applyEffects(dt);
		}
	}
}

void Game::processEffectsForPlayer(float dt){
	player.applyEffects(dt);
}

void Game::despawnEnemiesFromDeletedChunks(int direction){
	//0 is up
	//1 is right
	//2 is down
	//3 is left
	int min_x, max_x, min_y, max_y;
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

void Game::fixRemainingEnemyPosition(int direction){
	//0 up
	//1 right
	//2 down
	//3 left
	bool vert{false};
	if(direction % 2 == 0)
		vert = true;
	int adder = findAddingAmountOffsetWhenGeneratingChunks(direction);
	for(int i{}; i < board_enemies.size(); ++i){
		if(vert)
			board_enemies[i].position[1] += adder;
		else
			board_enemies[i].position[0] += adder;
	}
}

int Game::findAddingAmountOffsetWhenGeneratingChunks(int direction){
	switch(direction){
		case 0 :
		case 2 :
			return 5000;
		case 1 :
		case 3 :
			return -5000;
	}
}

void Game::moveAllProjectiles(float dt){
	for(int i{}; i < player_projectiles.size(); ++i){
		player_projectiles[i].move(dt);
	}
	for(int i{}; i < enemy_projectiles.size(); ++i){
		enemy_projectiles[i].move(dt);
	}
}

void Game::despawnProjectilesFromDeletedChunks(int direction){
	//0 is up
	//1 is right
	//2 is down
	//3 is left
	int min_x, max_x, min_y, max_y;
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

void Game::fixRemainingProjectilePosition(int direction){
	bool vert{false};
	if(direction % 2 == 0)
		vert = true;
	int adder = findAddingAmountOffsetWhenGeneratingChunks(direction);
	for(int i{}; i < board_enemies.size(); ++i){
		if(vert){
			player_projectiles[i].position[1] += adder;
			enemy_projectiles[i].position[1] += adder;
		}else{
			player_projectiles[i].position[0] += adder;
			enemy_projectiles[i].position[0] += adder;
		}
	}
}