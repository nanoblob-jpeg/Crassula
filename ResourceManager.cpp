#include "ResourceManager.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture> ResourceManager::Textures;
std::map<std::string, Chunk> ResourceManage::Chunks;

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

void ResourceManager::LoadGameObject(const char *file){
    std::vector<std::string> gameobject_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            gameobject_list.push_back(line);
        }
    }else{
        std::cout << "gameObjectDirectory could not be found/opened";
    }

    for(int i{}; i < gameobject_list.size(); ++i){
        std::string line2;
        std::ifstream fstream2(gameobject_list[i]);
        std::string vname;
        glm::vec2 pos, size;
        std::string textureName;
        glm::vec3 color;
        if(fstream){
            std::getline(fstream2, line2);
            vname = line2;
            std::getline(fstream2, line2);
            pos[0] = std::stof(line2);
            std::getline(fstream2, line2);
            pos[1] = std::stof(line2);
            std::getline(fstream2, line2);
            size[0] = std::stof(line2);
            std::getline(fstream2, line2);
            size[1] = std::stof(line2);
            std::getline(fstream2, line2);
            color[0] = std::stof(line2);
            std::getline(fstream2, line2);
            color[1] = std::stof(line2);
            std::getline(fstream2, line2);
            color[2] = std::stof(line2);
            std::getline(fstream2, textureName);
            GameObject obj(pos, size, ResourceManager::GetTexture(textureName.c_str()), color);
            Objects[vname] = obj;
        }else{
            std::cout << "could not open object file";
        }
    }
}

GameObject& GetGameObject(std::string name){
    return Objects[name];
}

void ResourceManager::LoadChunk(const char *chunkFile, bool list){
    if(bool){
        //parsing list of chunks for the 10x10 squares
        std::vector<std::string> chunk_list;
    
        std::string line;
        std::ifstream fstream(chunkFile);
        if(fstream){
            while(std::getline(fstream, line)){
                chunk_list.push_back(line);
            }
        }else{
            std::cout << "File not opened";
        }
        //loading 10x10 squares
        for(int i{}; i < chunk_list.size(); ++i){
            Chunks temp;
            temp.Load(chunk_list[i].c_str());
            Chunks[temp.name] = temp;
        }
    }else{
        Chunks temp;
        temp.Load(chunkFile);
        Chunks[temp.name] = temp;
    }
}

Chunk& ResourceManager::GetChunk(std::string name){
    return Chunks[name];
}