#include "Texture.h"
#include <iostream>

Texture::Texture() : m_width(0), m_height(0), m_internal_format(GL_RGB), m_image_format(GL_RGB), m_wrap_s(GL_REPEAT), m_wrap_t(GL_CLAMP_TO_EDGE), m_filter_min(GL_LINEAR), m_filter_max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

Texture::Texture(unsigned int width, unsigned int height, unsigned int depth):m_width{width}, m_height{height}, m_internal_format(GL_RGB), m_image_format(GL_RGB), m_wrap_s(GL_REPEAT), m_wrap_t(GL_CLAMP_TO_EDGE), m_filter_min(GL_LINEAR), m_filter_max(GL_LINEAR){
    glGenTextures(1, &this->ID);
}

void Texture::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->m_width = width;
    this->m_height = height;
    // create Texture
    glBindTexture(GL_TEXTURE_2D, this->ID);
    glTexImage2D(GL_TEXTURE_2D, 0, this->m_internal_format, width, height, 0, this->m_image_format, GL_UNSIGNED_BYTE, data);
    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->m_wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->m_wrap_t);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->m_filter_min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->m_filter_max);
    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::generateArray(unsigned char* data, int depth){
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, m_width, m_height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, this->index, this->m_width, this->m_height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, this->m_wrap_s);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, this->m_wrap_t);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, this->m_filter_min);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, this->m_filter_max);
    index++;
}

void Texture::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

void Texture::BindArray() const{
    glBindTexture(GL_TEXTURE_2D_ARRAY, this->ID);
}