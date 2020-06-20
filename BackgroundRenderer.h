#ifndef BACKGROUNDRENDERER
#define BACKGROUNDRENDERER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"

class BackgroundRenderer{
private:
	Shader m_shader;
	unsigned int m_quadVAO;
	unsigned int offsetVBO;
public:
	BackgroundRenderer(Shader &shader);
	~BackgroundRenderer();

	void DrawSprite(Texture &texture, glm::vec2 position);
	void setOffset(glm::vec2 &offset);
	void setViewMatrix(const std::string &name, glm::mat4 &view);
private:
	void initRenderData();
};
#endif