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
public:
	BackgroundRenderer(Shader &shader);
	~BackgroundRenderer();

	void DrawSprite(Texture &texture);
	void setOffset(glm::vec2 offset);
private:
	void initRenderData();
}
#endif