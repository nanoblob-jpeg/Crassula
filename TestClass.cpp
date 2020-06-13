#include "TestClass.h"
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

TestClass::TestClass(unsigned int width, unsigned int height){
	m_state = GAME_ACTIVE_CLASSIC;
	Width = width;
	Height = height;
	player = new Player();
}

TestClass::~TestClass(){
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