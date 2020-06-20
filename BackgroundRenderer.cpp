#include "BackgroundRenderer.h"

BackgroundRenderer::BackgroundRenderer(Shader &shader){
	this->m_shader = shader;
	this->initRenderData();
}

BackgroundRenderer::~BackgroundRenderer(){
	delete &m_shader;
}

void BackgroundRenderer::DrawSprite(Texture &texture){
	this->m_shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-7500.0f, 7500.0f, 0.0f));
	model = glm::scale(model, glm::vec3(15000.0f, 15000.0f, 1.0f));

	this->m_shader.setMat4("model", model);
	this->m_shader.setVec3("spriteColor", glm::vec3(1.0f));

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
		0.0f, 1.0f, -5.0f, 5.0f,
		1.0f, 0.0f, 5.0f, -5.0f,
		0.0f, 0.0f, -5.0f, -5.0f,

		0.0f, 1.0f, -5.0f, 5.0f,
		1.0f, 1.0f, 5.0f, 5.0f,
		1.0f, 0.0f, 5.0f, -5.0f
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
}

void BackgroundRenderer::setOffset(glm::vec2 &offset){
	m_shader.setVec2("offset", offset);
}