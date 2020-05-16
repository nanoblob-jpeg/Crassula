#ifndef SPRITERENDERER
#define SPRITERENDERER
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Shader.h"

class SpriteRenderer
{
private:
	Shader m_shader;
	unsigned int m_quadVAO;
public:
	SpriteRenderer(Shader &shader);
	~SpriteRenderer();

	void DrawSprite(
		Texture &texture, 
		glm::vec2 position, 
		glm::vec2 size = glm::vec2(10.0f, 10.0f), 
		float rotate = 0.0f, 
		glm::vec3 color = glm::vec3(1.0f)
	);
private:
	void initRenderData();
};
#endif