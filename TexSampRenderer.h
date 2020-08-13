#ifndef TEXSAMPRENDERER
#define TEXSAMPRENDERER

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "ResourceManager.h"
#include "Shader.h"

class TexSampRenderer{
private:
	Shader m_shader;
	unsigned int quadVAO;
	unsigned int instanceVBO;
	unsigned int instanceTexturePosVBO;
public:
	TexSampRenderer(Shader &shader);
	~TexSampRenderer();

	void DrawSprites(
		int num,
		Texture &texture,
		float size,
		glm::vec2 position = glm::vec2(0.0f, 0.0f)
	);
	void DrawSprites(int num, Texture &texture,glm::vec2 size, glm::vec2 position = glm::vec2(0.0f, 0.0f));

	void setOffset(glm::vec2 *data, int numOfOffsets);
	void setTextureCoords(float *data, int numOfCoords);
	void setViewMatrix(const std::string &name, glm::mat4 &viewMatrix) const;
private:
	void initRenderData();
};
#endif