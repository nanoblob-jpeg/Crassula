# Crassula
This 2D platformer takes you through an endless world while you collect plants to help you blast your way through enemies. With 49 plants and 5 bowls, there are many different ways to play

## CURRENTLY UNFINISHED; I AM JUST WAITING FOR MY AMAZING ARTIST TO FINISH UP DRAWING UP SPRITES :)
### the ones currently in the game are just filler textures

## Prerequisites/Installing
All of the prerequisites are included with this project so all you have to do is clone this project.
They include the libraries:
  - glad
  - GLFW
  - glm
  - irrklang

To install this project, clone it then run the game by running Crassula.exe. If that file does not exist then run a.exe.
You can recompile the program by using the following mingw commands or its equivalent in your terminal:
```
gcc glad.c -c
g++ -std=c++17 glad.o Background.cpp BackgroundRenderer.cpp Camera.cpp Chunks.cpp Game.cpp GameObject.cpp image.cpp main.cpp Plant.cpp Projectile.cpp Player.cpp ResourceManager.cpp Shader.cpp SpriteRenderer.cpp TexSampRenderer.cpp Texture.cpp -o Crassula.exe -lglfw3 -lirrklang -L./ -I./lib
```

## Notes
As I wanted to make it so that people could alter the game and change it as they pleased, at least to the extent of their copy on their machines, all of the data is stored in text files in the bin folder. You can see the formatting of each file if you look in the ResourceManager.cpp file except for chunks and players. You can find these in their respective Chunks.cpp and Player.cpp file.

## What I learned
This was my first time working with OpenGL and it makes me appreciate all of the work that game engines do behind the scenes. Besides that, the hardest part of this game was honestly creating all of the stuff in the game. Just trying to think of 50 plants took a lot of time and after a bit, none of the plants were special or distinct from each other. Furthermore, there were so many features I wanted to add at the start of development that just did not make it into game. This was due to the fact that the architecture that I had made for the game was not able to accomadate those features. Through this, I've learned that planning is really important, especially when taking on a large-ish project or one where multiple different parts have to interact with each other.

## To Do
Organize files better and clean up/optimize code. Also need to implement status effect icons.
