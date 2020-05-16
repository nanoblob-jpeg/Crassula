#include "ResourceManager.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture> ResourceManager::Textures;

// loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
Shader& ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name){
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
};
// retrieves a stored sader
Shader& ResourceManager::GetShader(std::string name){
    return Shaders[name];
};
// loads (and generates) a texture from file
Texture& ResourceManager::LoadTexture(const char *file, bool alpha, std::string name){
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
};
// retrieves a stored texture
Texture& ResourceManager::GetTexture(std::string name){
    return Textures[name];
};
// properly de-allocates all loaded resources
void ResourceManager::Clear(){
    for(auto iter : Shaders){
        glDeleteProgram(iter.second.ID);
    }
    for(auto iter : Textures){
        glDeleteTextures(1, &iter.second.ID);
    }
};
// loads and generates a shader from file
Shader ResourceManager::loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile){
    // retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // open files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        // close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();
    const char *gShaderCode = geometryCode.c_str();
    // 2. now create shader object from source code
    Shader shader;
    shader.compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
};
// loads a single texture from file
Texture ResourceManager::loadTextureFromFile(const char *file, bool alpha){
    // create texture object
    Texture texture;
    if (alpha)
    {
        texture.m_internal_format = GL_RGBA;
        texture.m_image_format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
};