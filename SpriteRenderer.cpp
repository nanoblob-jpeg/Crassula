#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Shader &shader){
	this->m_shader = shader;
	this->initRenderData();
}

SpriteRenderer::~SpriteRenderer(){
	glDeleteVertexArrays(1, &this->m_quadVAO);
}

void SpriteRenderer::initRenderData(){
	unsigned int VBO;
	float vertices[] = {
		// pos       // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->m_quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->m_quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &this->instanceVBO);
}

void SpriteRenderer::DrawSprite(Texture &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color){
	this->m_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	//these translations make it so we translate around the center, not a corner
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->m_shader.setMat4("model", model);
	this->m_shader.setVec3("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::bindInstanceBuffer(glm::vec2 *data, int numOfBlocks){
	glBindVertexArray(this->m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * numOfBlocks, data, GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glBindVertexArray(0);
}

void SpriteRenderer::setViewMatrix(const std::string &name, glm::mat4 &viewMatrix) const{
	m_shader.setMat4(name, viewMatrix);
}

void SpriteRenderer::DrawInstancedSprites(int num, Texture &texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color){
	this->m_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));

	//these translations make it so we translate around the center, not a corner
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

	model = glm::scale(model, glm::vec3(size, 1.0f));

	this->m_shader.setMat4("model", model);
	this->m_shader.setVec3("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->m_quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num); 
	glBindVertexArray(0);
}