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

	
}

Game::~Game(){
	delete Renderer;
}

void Game::Render(){
	Renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200.0f, 200.0f), glm::vec2(50.0f, 50.0f),
			45.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	
}

void Game::Update(float dt){
	//need to add moving logic here before the collision check logic
	cam.ProcessKeyboard(player.velocity, dt);

	if(m_state == HOME_MAIN){
		//don't know what I'm doing here, need to set it to the chunk that is displayed
		Chunk *home_main = &ResourceManager::GetChunk("home_main");
		player.interact = nullptr;

		for(int i{}; i < home_main->objects.size(); ++i){
			if(home_main->objects[i]->interactable){
				//checking for collision
				bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= home_main->objects[i]->position[0]
					&& home_main->objects[i]->position[0] + home_main->objects[i]->size[0] >= cam.Position[0] - player.bowl->size[0]/2;
				bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= home_main->objects[i]->position[0]
					&& home_main->objects[i]->position[1] + home_main->objects[i]->size[1] >= cam.Position[1] - player.bowl->size[1]/2;
				if(collisionX && collisionY){
					//setting the object as the one that the player can interact with
					player.interact = home_main->objects[i];
				} 
			}else{
				bool collisionX = cam.Position[0] + player.bowl->size[0]/2 >= home_main->objects[i]->position[0]
					&& home_main->objects[i]->position[0] + home_main->objects[i]->size[0] >= cam.Position[0] - player.bowl->size[0]/2;
				bool collisionY = cam.Position[1] + player.bowl->size[1]/2 >= home_main->objects[i]->position[0]
					&& home_main->objects[i]->position[1] + home_main->objects[i]->size[1] >= cam.Position[1] - player.bowl->size[1]/2;
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
					if(prevPosition[1] + player.bowl->size[1]/2 < home_main->objects[i]->position[1]){
						//testing to see if the player with clearly above the block
						if(-player.bowl->size[0]/2 < prevPosition[0] - home_main->objects[i]->position[0] < home_main->objects[i]->size[0] + player.bowl->size[0]/2){
							direction = 0;
						}else{
							//testing the edge cases where the player is in the corner of the space around the block
							//yet still collides with it this frame
							//uses distance for each axis and the velocity in order to figure out which one
							//hit first
							float distanceX = prevPosition[0] < home_main->objects[i]->position[0]? home_main->objects[i]->position[0] - (prevPosition[0] + player.bowl->size[0]/2) 
								: home_main->objects[i]->position[0] + home_main->objects[i]->size[0] - (prevPosition[0] - player.bowl->size[0]/2);
							float distanceY = home_main->objects[i]->position[1] - (prevPosition[1] + player.bowl->size[1]/2);

							if(distanceX/player.velocity[0] < distanceY/(-player.velocity[1])){
								if(prevPosition[0] < home_main->objects[i]->position[0]){
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
					else if(prevPosition[1] - player.bowl->size[1]/2 > home_main->objects[i]->position[1] + home_main->objects[i]->size[1]){
						//testing to see if the player with clearly above the block
						if(-player.bowl->size[0]/2 < prevPosition[0] - home_main->objects[i]->position[0] < home_main->objects[i]->size[0] + player.bowl->size[0]/2){
							direction = 2;
						}else{
							//testing the edge cases where the player is in the corner of the space around the block
							//yet still collides with it this frame
							//uses distance for each axis and the velocity in order to figure out which one
							//hit first
							float distanceX = prevPosition[0] < home_main->objects[i]->position[0]? home_main->objects[i]->position[0] - (prevPosition[0] + player.bowl->size[0]/2) 
								: home_main->objects[i]->position[0] + home_main->objects[i]->size[0] - (prevPosition[0] - player.bowl->size[0]/2);
							float distanceY = prevPosition[1] - player.bowl->size[1]/2 - home_main->objects[i]->position[1];

							if(distanceX/player.velocity[0] < distanceY/(player.velocity[1])){
								if(prevPosition[0] < home_main->objects[i]->position[0]){
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
					else if(prevPosition[0] < home_main->objects[i]->position[0]){
						direction = 3;
					}else{
						direction = 1;
					}
					
					//applying the corrections to the players position
					//while also fixing the velocity in that direction
					//to make it seem like they were stopped by the object
					if(direction == 0){
						cam.Position[1] -= cam.Position[1] + player.bowl->size[1]/2 - home_main->objects[i]->position[1];
						player.velocity[1] = 0.0f;
						player.falling = false;
					}else if(direction == 1){
						cam.Position[0] += home_main->objects[i]->position[0] + home_main->objects[i]->size[0] - (cam.Position[0] - player.bowl->size[0]/2);
						player.velocity[0] = 0.0f;
					}else if(direction == 2){
						cam.Position[1] += home_main->objects[i]->position[1] + home_main->objects[i]->size[1] - (cam.Position[1] - player.bowl->size[1]/2);
						player.velocity[1] = 0.0f;
					}else{
						cam.Position[0] -= cam.Position[0] + player.bowl->size[0]/2 - home_main->objects[i]->position[0];
						player.velocity[0] = 0.0f;
					}
				}
			}
		}
	}//test for the other states else if()
};

void Game::ProcessInput(float dt){
	if(m_state != START_SCREEN){
		if(Keys[GLFW_KEY_W]){
			if(upCounter == 0)
				player.falling = true;
			//need to add code in the collision detector that will change falling to false
			if(upCounter < 0.5){
				upCounter += dt;
				player.velocity.y = 3.5;
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
						for(int j{}; j < 3; ++j){
							generateChunk(i, j);
						}
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
	std::uniform_int_distribution chunkSelector{1,numOfChunks};
	std::uniform_int_distribution random{1, 100};
	std::vector<Chunk*> temp;
	for(int i{}; i < 100; ++i){
		temp.push_back(ResourceManager::GetChunk(std::to_string(chunkSelector(mersenne))));
	}
	//should add spawning logic here
	for(int i{}; i < 100; ++i){
		for(int j{10}; j < 100; ++j){
			if(temp[i].locationOfObjects[j]){
				if(!temp[i].locationOfObjects[j-10]){
					int rnum = random(mersenne);
					if(rnum <= 5){
						//spawn a plant
					}else if(rnum <= 30){
						//spawn an enemy
					}
				}
			}
		}
	}

	if(x == -1){
		if(board.size() == 3){
			board.push_front(std::deque<Chunk>{});
			for(int i{}; i < y; ++i){
				board[0].push_back(std::vector<Chunk*>{});
			}
			board[0].push_back(temp);
			for(int i = board[0].size(); i < board[1].size(); ++i){
				board[0].push_back(std::vector<Chunk*>{});
			}
		}else{
			board[0][y] = temp;
		}
	}else if(x == 3){
		if(board.size() == 3){
			board.push_back(std::deque<Chunk>{});
			for(int i{}; i < y; ++i){
				board[2].push_back(std::vector<Chunk*>{});
			}
			board[2].push_back(temp);
			for(int i = board[2].size(); i < board[1].size(); ++i){
				board[2].push_back(std::vector<Chunk*>{});
			}
		}else{
			board[2][y] = temp;
		}
	}else{
		board[x][y] = temp;
	}
}