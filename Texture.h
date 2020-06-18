#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture{
public:

	//texture ID
	unsigned int ID;

	//texture dimensions
	unsigned int m_width, m_height;

	//texture format
	unsigned int m_internal_format; //format of texture object
	unsigned int m_image_format; // format of loaded image

	//texture configuration
	unsigned int m_wrap_s; //wrapping on S axis
	unsigned int m_wrap_t; //wrapping on T axis
	unsigned int m_filter_min; //filtering mode if texture pixels < screen pixels
	unsigned int m_filter_max; //filtering mode if texture pixels > screen pixels

	//below is only for array textures
	int index = 0;

	//constructor
	Texture();
	Texture(unsigned int width, unsigned int height, unsigned int depth);

	// generates texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data);

	//generates texture layer for array textures
	void GenerateArray(unsigned char* data);

	//binds the texture as the current active GL_TEXTURE_2D texture object
	void Bind() const;
	void BindArray() const;

};
#endif