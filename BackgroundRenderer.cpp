#include "BackgroundRenderer.h"

BackgroundRenderer::BackgroundRenderer(Shader &shader){
	this->m_shader = shader;
	this->initRenderData();
}

BackgroundRenderer::~BackgroundRenderer(){
	delete &m_shader;
}

void BackgroundRenderer::DrawSprite(Texture &texture, glm::vec2 position){
	this->m_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-302 + position[0], -400 + position[1], 0.0f));
	model = glm::scale(model, glm::vec3(800.0f, 800.0f, 1.0f));

	glm::vec3 color(1.0f);
	this->m_shader.setMat4("model", model);
	this->m_shader.setVec3("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void BackgroundRenderer::initRenderData(){
	unsigned int VBO;
	float vertices[] = {
		// pos       // tex
		0.0f, 1.0f, 
		1.0f, 0.0f, 
		0.0f, 0.0f, 

		0.0f, 1.0f, 
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->m_quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->m_quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &this->offsetVBO);
}

void BackgroundRenderer::setOffset(glm::vec2 &offset){
	//these are the texturePositions for a display of 800 by 600 that has 
	//backgrounds the size of 2500 by 2500
	//these were calculated by:
	/*
	width: ((imageSize - displayWidth)/2)/imageSize
		   and 1 - (that^^) to get the right side
	height: ((imageSize - displayHeight)/2)/imageSize
			and 1- (that^^) for the top side
	*/
	float texturePositions[] = {
		0.34f + offset[0], 0.62f + offset[1],
		0.66f + offset[0], 0.38f + offset[1],
		0.34f + offset[0], 0.38f + offset[1],

		0.34f + offset[0], 0.62f + offset[1],
		0.66f + offset[0], 0.62f + offset[1],
		0.66f + offset[0], 0.38f + offset[1]
	};

	glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturePositions), texturePositions, GL_STATIC_DRAW);

	glBindVertexArray(this->m_quadVAO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BackgroundRenderer::setViewMatrix(const std::string &name, glm::mat4 &view){
	m_shader.setMat4(name, view);
}