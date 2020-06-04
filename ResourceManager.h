#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>
#include <vector>

#include <glad/glad.h>

#include "Texture.h"
#include "Shader.h"
#include "GameObject.h"
#include "Chunks.h"
#include "Bowl.h"
#include "Plant.h"
#include "Effect.h"
#include "Enemy.h"


class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture> Textures;
    static std::map<std::string, Chunk> Chunks;
    static std::map<std::string, GameObject> Objects;
    static std::map<std::string, Bowl> Bowls;
    static std::map<std::string, Plant> Plants;
    static std::map<std::string, Effect> Effects;
    static std::map<std::string, Enemy> Enemies;
    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader& LoadShader(const char *vShaderFilePlant, const char *fShaderFile, const char *gShaderFile, std::string name);
    // retrieves a stored sader
    static Shader& GetShader(std::string name);
    // loads textures from a file with many textures
    static void LoadTexture2(const char *file);
    // loads (and generates) a texture from file
    static Texture& LoadTexture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static Texture& GetTexture(std::string name);
    // loads a chunk from file, or file with list of files
    static void LoadChunk(const char *chunkFile, bool list);
    // returns a chunk
    static Chunk& GetChunk(std::string name);
    //loads GameObjects
    static void LoadGameObject(const char *file);
    // returns GameObjects
    static GameObject& GetGameObject(std::string name);
    //loads the bowls
    static void LoadBowl(const char *file);
    //returns a bowl
    static Bowl& GetBowl(std::string name);
    //loads the plants
    static void LoadPlant(const char *file);
    //returns a plant
    static Plant& GetPlant(std::string name);
    //loads the effects
    static void LoadEffect(const char *file);
    //returns an effect
    static Effect& GetEffect(std::string name);
    //need to load the enemy by just adding it to the map
    //returns an enemy
    static Enemy& GetEnemy(std::string name);
    //loads projectiles
    static void LoadProjectiles(const char *file);
    //returns a projectile
    static Projectile& GetProjectile(std::string name);
    // properly de-allocates all loaded resources
    static void Clear();
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads and generates a shader from file
    static Shader loadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    // loads a single texture from file
    static Texture loadTextureFromFile(const char *file, bool alpha);
};

#endif