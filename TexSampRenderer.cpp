#include "TexSampRenderer.h"

TexSampRenderer::TexSampRenderer(Shader &shader){
	this->m_shader = shader;
	this->initRenderData();
}

TexSampRenderer::~TexSampRenderer(){
	delete &m_shader;
}

void TexSampRenderer::initRenderData(){
	unsigned int VBO;
	float vertices[] = {
		// pos     
		0.0f, 1.0f, 
		1.0f, 0.0f, 
		0.0f, 0.0f, //top left

		0.0f, 1.0f, 
		1.0f, 1.0f, 
		1.0f, 0.0f  
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &this->instanceVBO);
	glGenBuffers(1, &this->instanceTexturePosVBO);
}

void TexSampRenderer::setOffset(glm::vec2 *data, int numOfOffsets){
	glBindVertexArray(this->quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numOfOffsets, data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(1, 1);
	glBindVertexArray(0);
}

void TexSampRenderer::setTextureCoords(glm::vec2 *data, int numOfCoords){
	glBindVertexArray(this->quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceTexturePosVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * numOfCoords, data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TexSampRenderer::setViewMatrix(const std::string &name, glm::mat4 &viewMatrix) const{
	m_shader.setMat4(name, viewMatrix);
}

void TexSampRenderer::DrawSprites(int num, Texture &texture,float size, glm::vec2 position){
	this->m_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(size, size, 1.0f));

	this->m_shader.setMat4("model", model);
	this->m_shader.setVec3("spriteColor", clm::vec3(0.0));

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, num);
	glBindVertexArray(0);
}