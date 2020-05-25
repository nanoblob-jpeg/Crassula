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
	//change this so that it runs on the one file names all of it and then 
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
}

Game::~Game(){
	delete Renderer;
}

void Game::Render(){
	if(m_state == HOME_MAIN){
		Renderer->DrawSprite(ResourceManager::GetTexture("face"),
			glm::vec2(200.0f, 200.0f), glm::vec2(50.0f, 50.0f),
				45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		//render the game stuff
	}
	
}

void Game::Update(float dt){
	//need to add moving logic here before the collision check logic
	cam.ProcessKeyboard(player.velocity, dt);

	if(m_state == HOME_MAIN){
		//don't know what I'm doing here, need to set it to the chunk that is displayed
		Chunk *home_main = &ResourceManager::GetChunk("home_main");
		player.interact = nullptr;

		for(int i{}; i < home_main->objects.size(); ++i){
			home_main_pAndOCollisions(home_main->objects[i], dt);
		}
	}else if(m_state == GAME_ACTIVE_CLASSIC){
		//finding out which chunk the player is in using position
		//the game is centered on the center of the middle section
		//we need a system in order to find the exact 10x10 player is in
		int height, width;
		if(cam.Position[1] / 500 == 0){
			if(cam.Position[1] < 0){
				height = -1;
			}else{
				height = 0;
			}
		}else{
			if(cam.Position[1] < 0){
				height = cam.Position[1]/500 -1;
			}else{
				height = cam.Position[1] / 500;
			}
		}
		if(cam.Position[0] / 500 == 0){
			if(cam.Position[0] < 0){
				width = -1;
			}else{
				width = 0;
			}
		}else{
			if(cam.Position[0] < 0){
				width = cam.Position[0]/500 -1;
			}else{
				width = cam.Position[0] / 500;
			}
		}

		//collision detection between player and objects(the ground/plants/gates)
		//since we have the player exact location, we only check the 9
		//chunks that are right around it

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
					game_classic_p_and_object_collisions(board[index_x][index_y][index_chunk].objects[j], gameobject_offset_x, gameobject_offset_y, dt);
				}
			}
		}



		//collision detection between player projectiles and enemies




		//collision detection between enemy projectile and player

	}


	//test for the other states else if()
};

void Game::ProcessInput(float dt){
	if(m_state != START_SCREEN){
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
		if(player.velocity.x > 0 && !player.velocity.x && !player.velocity.y){
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
					for(int i{}; i < 3; ++i){
						generateChunk(-1, 1);
						generateChunk(0, -1);
						generateChunk(0, -1);
					}

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

void Game::generateChunk(int x, int y){
	std::uniform_int_distribution chunkSelector{1,static_cast<int>(numOfChunks)};
	std::uniform_int_distribution random{1, 100};
	std::uniform_int_distribution plantPicker{0, static_cast<int>(numOfPlants) - 1};
	std::uniform_int_distribution enemyPicker{0, static_cast<int>(numOfEnemies) - 1};
	//creating a chunk to insert
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
						//5% chance to spawn in a plant
						//uses the plant object but puts a location
						//copies non-important data into the stuff
						int plantNum = plantPicker(mersenne);
						auto it = ResourceManager::Plants.begin();
						while(plantNum--){
							++it;
						}
						temp[i].plants.push_back((it)->second);
						temp[i].plants[temp[i].plants.size() - 1].position.x = j/10;
						temp[i].plants[temp[i].plants.size() - 1].position.y = j%10;
					}else if(rnum <= 30){
						//20% chance to spawn an enemy
						//stored inside of a vector for the Game class
						//might change this to have a certain percentage per enemy
						//and then spawn that enemy in
						int enemyNum = enemyPicker(mersenne);
						auto it = ResourceManager::Enemies.begin();
						while(enemyNum--)
							++it;
						board_enemies.push_back((it)->second);
						board_enemies[temp[i].plants.size() - 1].position.x = j/10;
						board_enemies[temp[i].plants.size() - 1].position.y = j%10;
					}
				}
			}
		}
	}
	//-1 means inserting before things
	//3 means appending to the end
	if(y == -1){
		if(x != -1 && x != 3){
			board[x].push_front(temp);
			board[x].pop_back();
		}else{
			if(x == -1){
				board.push_front(std::deque<std::vector<Chunk>> {temp});
				board.pop_back();
				board[0].push_front(temp);
				board[0].pop_back();
			}
			if(x == 3){
				board.push_back(std::deque<std::vector<Chunk>> {temp});
				board.pop_front();
				board[2].push_front(temp);
				board[2].pop_back();
			}
		}
	}else if(y == 3){
		if(x != -1 && x != 3){
			board[x].push_back(temp);
			board[x].pop_front();
		}else{
			if(x == -1){
				board.push_front(std::deque<std::vector<Chunk>> {temp, temp, temp});
				board.pop_back();
			}
			if(x == 3){
				board.push_back(std::deque<std::vector<Chunk>> {temp, temp, temp});
				board.pop_front();
			}
		}
	}else if(x == -1){
		//possible error here as it only inserts one area and not three 
		//like they might be expecting
		board.push_front(std::deque<std::vector<Chunk>> {temp, temp, temp});
		board.pop_back();
	}else if(x == 3){
		board.push_back(std::deque<std::vector<Chunk>> {temp, temp, temp});
		board.pop_front();
	}else{
		board[x][y] = temp;
	}
}

void Game::loadEnemies(){
	//create the basic enemy here except don't have position set
	//then save it to the map as a broad class enemy not
	//as the specific one
	//then they select it and set the position
}

void Game::home_main_pAndOCollisions(GameObject *test, float dt){
	if(test->interactable){
		//checking for collision
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= test->position[0]
			&& test->position[0] + test->size[0] >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= test->position[1]
			&& test->position[1] + test->size[1] >= cam.Position[1] - player.bowl->size[1]/2;
		if(collisionX && collisionY){
			//setting the object as the one that the player can interact with
			player.interact = test;
		} 
	}else{
		bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= test->position[0]
			&& test->position[0] + test->size[0] >= cam.Position[0] - player.bowl->size[0]/2;
		bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= test->position[1]
			&& test->position[1] + test->size[1] >= cam.Position[1] - player.bowl->size[1]/2;
		if(collisionX && collisionY){
			//need to also change the velocity of the player here to be zero in the direction
			//of the collision
			//getting previous location
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
			if(prevPosition[1] + player.bowl->size[1]/2 < test->position[1]){
				//testing to see if the player with clearly above the block
				if(-player.bowl->size[0]/2 < prevPosition[0] - test->position[0] < test->size[0] + player.bowl->size[0]/2){
					direction = 0;
				}else{
					//testing the edge cases where the player is in the corner of the space around the block
					//yet still collides with it this frame
					//uses distance for each axis and the velocity in order to figure out which one
					//hit first
					float distanceX = prevPosition[0] < test->position[0]? test->position[0] - (prevPosition[0] + player.bowl->size[0]/2) 
						: test->position[0] + test->size[0] - (prevPosition[0] - player.bowl->size[0]/2);
					float distanceY = test->position[1] - (prevPosition[1] + player.bowl->size[1]/2);

					if(distanceX/player.velocity[0] < distanceY/(-player.velocity[1])){
						if(prevPosition[0] < test->position[0]){
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
			else if(prevPosition[1] - player.bowl->size[1]/2 > test->position[1] + test->size[1]){
				//testing to see if the player with clearly above the block
				if(-player.bowl->size[0]/2 < prevPosition[0] - test->position[0] < test->size[0] + player.bowl->size[0]/2){
					direction = 2;
				}else{
					//testing the edge cases where the player is in the corner of the space around the block
					//yet still collides with it this frame
					//uses distance for each axis and the velocity in order to figure out which one
					//hit first
					float distanceX = prevPosition[0] < test->position[0]? test->position[0] - (prevPosition[0] + player.bowl->size[0]/2) 
						: test->position[0] + test->size[0] - (prevPosition[0] - player.bowl->size[0]/2);
					float distanceY = prevPosition[1] - player.bowl->size[1]/2 - test->position[1];

					if(distanceX/player.velocity[0] < distanceY/(player.velocity[1])){
						if(prevPosition[0] < test->position[0]){
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
			else if(prevPosition[0] < test->position[0]){
				direction = 3;
			}else{
				direction = 1;
			}
			
			//applying the corrections to the players position
			//while also fixing the velocity in that direction
			//to make it seem like they were stopped by the object
			if(direction == 0){
				cam.Position[1] -= cam.Position[1] + player.bowl->size[1]/2 - test->position[1];
				player.velocity[1] = 0.0f;
				player.falling = false;
			}else if(direction == 1){
				cam.Position[0] += test->position[0] + test->size[0] - (cam.Position[0] - player.bowl->size[0]/2);
				player.velocity[0] = 0.0f;
			}else if(direction == 2){
				cam.Position[1] += test->position[1] + test->size[1] - (cam.Position[1] - player.bowl->size[1]/2);
				player.velocity[1] = 0.0f;
			}else{
				cam.Position[0] -= cam.Position[0] + player.bowl->size[0]/2 - test->position[0];
				player.velocity[0] = 0.0f;
			}
		}
	}
}

void Game::game_classic_p_and_object_collisions(GameObject *object, int gameobject_offset_x, int gameobject_offset_y, float dt){
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
			//need to also change the velocity of the player here to be zero in the direction
			//of the collision
			//getting previous location
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
				if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) < object->size[0] + gameobject_offset_x + player.bowl->size[0]/2){
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
				if(-player.bowl->size[0]/2 < prevPosition[0] - (object->position[0] + gameobject_offset_x) < object->size[0] + player.bowl->size[0]/2){
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