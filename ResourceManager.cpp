#include "ResourceManager.h"
#include "stb_image.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <tuple>

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture> ResourceManager::Textures;
std::map<std::string, Chunk> ResourceManager::Chunks;
std::map<std::string, GameObject> ResourceManager::Objects;
std::map<std::string, std::pair<Bowl, int>> ResourceManager::Bowls;
std::map<std::string, Plant> ResourceManager::Plants;
std::map<std::string, Effect> ResourceManager::Effects;
std::map<std::string, Enemy> ResourceManager::Enemies;
std::map<std::string, Projectile> ResourceManager::Projectiles;
std::map<std::string, Background> ResourceManager::Backgrounds;
std::map<std::string, int> ResourceManager::textureDepth;

// loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
Shader& ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name){
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
};
// retrieves a stored sader
Shader& ResourceManager::GetShader(std::string name){
    return Shaders[name];
};
// loads and generates a texture from a file that contains many textures
void ResourceManager::LoadTexture2(const char *file){
    std::vector<std::tuple<std::string, bool, std::string>> texturePaths;
    /*
    file format:
    path name
    alpha bool(either 0 or 1, zero for false, 1 for true)
    name
    (repeats)
    */
    std::ifstream fstream(file);
    std::string line;
    if(fstream){
        while(std::getline(fstream, line)){
            std::string path = line;
            std::getline(fstream, line);
            bool alpha = line.c_str()[0] - '0';
            std::getline(fstream, line);
            std::string name = line;
            texturePaths.push_back(std::make_tuple(path, alpha, name));
        }
    }

    for(int i{}; i < texturePaths.size(); ++i){
        LoadTexture(std::get<0>(texturePaths[i]).c_str(), std::get<1>(texturePaths[i]), std::get<2>(texturePaths[i]));
    }
}
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
    for(auto iter : Chunks){
        delete &iter.second;
    }
    for(auto iter : Objects){
        delete &iter.second;
    }
    for(auto iter : Bowls){
        delete &iter.second;
    }
    for(auto iter : Plants){
        delete &iter.second;
    }
    for(auto iter : Effects){
        delete &iter.second;
    }
    for(auto iter : Projectiles){
        delete &iter.second;
    }
    for(auto iter : Backgrounds){
        delete &iter.second;
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
    fstream.close();

    /*
    format of the file:
    name
    pos y
    pos x
    height
    width
    color r
    color g
    color b
    texture name

    position here should be in reference to the 10 by 10 grid most of the time
    for special screens like home_main, it will be in reference to that grid
    each square on the grid is equal to 50 by 50 pixels
        changed it so that position is reflective to the position is by pixel, easier for collision
    since position is stored as a float, if need be, it can also be use as
        partial grid lengths in order to place things better
    name for the common building blocks is going to be their position in the
        x y grid ie, the one in index 0,1 will be named 01
    */
    for(int i{}; i < gameobject_list.size(); ++i){
        std::string line2;
        std::ifstream fstream2(gameobject_list[i].c_str());
        std::string vname;
        glm::vec2 pos, size;
        std::string textureName;
        glm::vec3 color;
        if(fstream2){
            std::getline(fstream2, vname);
            std::getline(fstream2, line2);
            pos[1] = std::stof(line2);
            std::getline(fstream2, line2);
            pos[0] = std::stof(line2);
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
            std::cout << gameobject_list[i] << '\n';
            std::cout << "could not open object file";
        }
    }
}

GameObject& ResourceManager::GetGameObject(std::string name){
    return Objects[name];
}

void ResourceManager::LoadChunk(const char *chunkFile, bool list){
    if(list){
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
            Chunk temp;
            temp.Load(chunk_list[i].c_str());
            Chunks[temp.name] = temp;
        }
    }else{
        Chunk temp;
        temp.Load(chunkFile);
        Chunks[temp.name] = temp;
    }
}

Chunk& ResourceManager::GetChunk(std::string name){
    return Chunks[name];
}

void ResourceManager::LoadBowl(const char *file){
    std::vector<std::string> bowl_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            bowl_list.push_back(line);
        }
    }else{
        std::cout << "Could not open bowl list file";
    }

    fstream.close();

    /*
    format for bowl files:
    name
    health
    defense
    attack
    attackSpeed
    speed
    recovery
    luck
    width
    height
    numOfPlants
    numOfLevelFour
    number of attack animation frames
    texture names of attack animations
    */
    int counter = 0;
    for(int i{}; i < bowl_list.size(); ++i){
        std::ifstream fstream2(bowl_list[i]);
        std::string name;
        //todo fix variables
        int health, damage, attack, num, numOfPlants, numOfLevelFour;
        float attackSpeed, speed, recovery, luck;
        glm::vec2 size;
        std::getline(fstream2, name);
        std::getline(fstream2, line);
        health = std::stoi(line);
        std::getline(fstream2, line);
        damage = std::stoi(line);
        std::getline(fstream2, line);
        attack = std::stoi(line);
        std::getline(fstream2, line);
        attackSpeed = std::stof(line);
        std::getline(fstream2, line);
        speed = std::stof(line);
        std::getline(fstream2, line);
        recovery = std::stof(line);
        std::getline(fstream2, line);
        luck = std::stof(line);
        std::getline(fstream2, line);
        size[0] = std::stoi(line);
        std::getline(fstream2, line);
        size[1] = std::stoi(line);
        std::getline(fstream2, line);
        numOfPlants = std::stoi(line);
        std::getline(fstream2, line);
        numOfLevelFour = std::stoi(line);
        std::getline(fstream2, line);
        num = std::stoi(line);
        Bowl temp = Bowl(name, health, damage, attack, attackSpeed, speed, recovery, luck, numOfPlants, numOfLevelFour, size);
        for(int j{}; j < num; ++j){
            std::getline(fstream2, line);
            temp.attackAnimation.push_back(Textures[line]);
        }
        Bowls[name] = std::pair<Bowl, int>{temp, counter};
        fstream2.close();
        ++counter;
    }
}

std::pair<Bowl, int>& ResourceManager::GetBowl(std::string name){
    return Bowls[name];
}

void ResourceManager::LoadPlant(const char *file){
    std::vector<std::string> plant_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            plant_list.push_back(line);
        }
    }else{
        std::cout << "Could not open plant list file";
    }

    fstream.close();

    /*
    format for plant files:
    name
    Texture name
    projectile name
    projectile name 2
    projectile name 3
    projectile name 4
    width
    height
    color.r
    color.g
    color.b
    */
    //texture width needs to be top left corner
    //needs also to be specified starting from the lower left hand corner of the texture
    for(int i{}; i < plant_list.size(); ++i){
        std::ifstream fstream2(plant_list[i]);
        std::string name, pname, pname2, pname3, pname4;
        int num;
        glm::vec2 size;
        glm::vec3 color;
        if(fstream2){
            std::getline(fstream2, name);
            std::getline(fstream2, pname);
            std::getline(fstream2, pname2);
            std::getline(fstream2, pname3);
            std::getline(fstream2, pname4);
            std::getline(fstream2, line);
            size[0] = std::stof(line);
            std::getline(fstream2, line);
            size[1] = std::stof(line);
            std::getline(fstream2, line);
            color[0] = std::stoi(line);
            std::getline(fstream2, line);
            color[1] = std::stoi(line);
            std::getline(fstream2, line);
            color[2] = std::stof(line);
            Plant temp = Plant(name, name, size, color);
            temp.projectileName.push_back(pname);
            temp.projectileName.push_back(pname2);
            temp.projectileName.push_back(pname3);
            temp.projectileName.push_back(pname4);

            Plants[name] = temp;
            fstream2.close();
        }else{
            std::cout << "couldn't open plant file";
        }
    }
}

Plant& ResourceManager::GetPlant(std::string name){
    return Plants[name];
}

void ResourceManager::LoadEffect(const char *file){
    std::vector<std::string> effect_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            effect_list.push_back(line);
        }
    }else{
        std::cout << "Could not open effect list file";
    }

    fstream.close();

    /*
    for values that the effect does not effect, should be 0
    negative is a debuff, positive is a buff
    format for effect files:
    name
    dehealth
    dedefense
    deattack
    despeed
    derecovery
    time
    frameTimeCounter
    */

    for(int i{}; i < effect_list.size(); ++i){
        std::ifstream fstream2(effect_list[i]);
        std::string name;
        int dehealth, dedefense, deattack;
        float despeed, derecovery, time, frameTimeCounter;
        std::getline(fstream2, name);
        std::getline(fstream2, line);
        dehealth = std::stoi(line);
        std::getline(fstream2, line);
        dedefense = std::stoi(line);
        std::getline(fstream2, line);
        deattack = std::stoi(line);
        std::getline(fstream2, line);
        despeed = std::stof(line);
        std::getline(fstream2, line);
        derecovery = std::stof(line);
        std::getline(fstream2, line);
        time = std::stof(line);
        std::getline(fstream2, line);
        frameTimeCounter = std::stof(line);
        Effects[name] = Effect(name, dehealth, dedefense, deattack, despeed, derecovery, time, frameTimeCounter);
        fstream2.close();
    }
}

Effect& ResourceManager::GetEffect(std::string name){
    return Effects[name];
}

Enemy& ResourceManager::GetEnemy(std::string name){
    return Enemies[name];
}

void ResourceManager::LoadProjectiles(const char *file){
    std::vector<std::string> projectile_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            projectile_list.push_back(line);
        }
    }else{
        std::cout << "Could not open projectile list file";
    }

    fstream.close();

    /*
    format for projectile files:
    name
    damage
    numBullet
    range
    piercing
    speed
    width
    height
    effect num
    effects
    */
    //texture width needs to be top left corner
    //needs also to be specified starting from the lower left hand corner of the texture
    for(int i{}; i < projectile_list.size(); ++i){
        std::ifstream fstream2(projectile_list[i]);
        std::string name;
        int damage, range, num, numBullet;
        bool piercing;
        float speed;
        glm::vec2 size;
        std::getline(fstream2, name);
        std::getline(fstream2, line);
        damage = std::stoi(line);
        std::getline(fstream2, line);
        numBullet = std::stoi(line);
        std::getline(fstream2, line);
        range = std::stoi(line);
        std::getline(fstream2, line);
        piercing = static_cast<bool>(std::stoi(line));
        std::getline(fstream2, line);
        speed = std::stof(line);
        std::getline(fstream2, line);
        size[0] = std::stof(line);
        std::getline(fstream2, line);
        size[1] = std::stof(line);
        Projectile temp = Projectile(size, damage, range, name, numBullet, speed, piercing);
        std::getline(fstream2, line);
        num = std::stoi(line);
        for(int i{}; i < num; ++i){
            std::getline(fstream2, line);
            temp.addEffect(ResourceManager::Effects[line]);
        }
        ResourceManager::Projectiles[name] = temp;
        fstream2.close();
    }
}

Projectile& ResourceManager::GetProjectile(std::string name){
    return Projectiles[name];
}

void ResourceManager::LoadBackgrounds(const char *file){
    std::vector<std::string> background_list;
    std::string line;
    std::ifstream fstream(file);
    if(fstream){
        while(std::getline(fstream, line)){
            background_list.push_back(line);
        }
    }else{
        std::cout << "Could not open effect list file";
    }

    fstream.close();

    /*
    name
    name1
    name2
    name3
    */

    for(int i{}; i < background_list.size(); ++i){
        std::ifstream fstream2(background_list[i]);
        std::string name, name1, name2, name3;
        std::getline(fstream2, name);
        std::getline(fstream2, name1);
        std::getline(fstream2, name2);
        std::getline(fstream2, name3);
        Backgrounds[name] = Background(name, name1, name2, name3);
        fstream2.close();
    }
}

Background& ResourceManager::GetBackground(std::string name){
    return Backgrounds[name];
}

void ResourceManager::LoadArrayTextures(const char *file){
    std::vector<std::string> texturePaths;
    std::vector<std::string> names;
    /*
    file format:
    name
    path name
    (repeats)
    */
    std::ifstream fstream(file);
    std::string line;
    if(fstream){
        while(std::getline(fstream, line)){
            names.push_back(line);
            std::getline(fstream, line);
            texturePaths.push_back(line);

        }
    }

    for(int i{}; i < texturePaths.size(); ++i){
        Textures[names[i]] = LoadArrayTexture(texturePaths[i]);
    }
}

Texture ResourceManager::LoadArrayTexture(std::string directory){
    /*


    all of these must be in GL_RGBA format aka .png or else this will break


    */
    std::vector<std::string> textureDir;
    std::vector<std::string> names;
    /*
    file format:
    max width
    max height
    number of textures
    name
    path name
    name
    path name
    etc.
    (repeats)
    */
    std::ifstream fstream(directory.c_str());
    std::string line;
    unsigned int width, height, depth;
    if(fstream){
        std::getline(fstream, line);
        width = static_cast<unsigned int>(std::stoi(line));
        std::getline(fstream, line);
        height = static_cast<unsigned int>(std::stoi(line));
        std::getline(fstream, line);
        depth = static_cast<unsigned int>(std::stoi(line));
        while(std::getline(fstream, line)){
            names.push_back(line);
            std::getline(fstream, line);
            textureDir.push_back(line);
        }
    }

    Texture texture(width, height, depth);
    for(int i{}; i < textureDir.size(); ++i){
        int texWidth, texHeight, nrChannels;
        unsigned char* data = stbi_load(textureDir[i].c_str(), &texWidth, &texHeight, &nrChannels, 0);
        texture.generateArray(data, depth);
        textureDepth[names[i]] = i;
        stbi_image_free(data);
    }
    return texture;
}

int ResourceManager::getDepth(std::string name){
    return textureDepth[name];
}

void ResourceManager::setDepth(std::string name, int depth){
    textureDepth[name] = depth;
}