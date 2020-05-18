#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
  
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader() {};
    // use/activate the shader
    void use();
    //compiles shader from given source code
    void compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource = nullptr); // note: geometry source code is optional 
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, glm::vec3 &vec) const;
    void setMat4(const std::string &name, glm::mat4 &mat)const;
    void setMat(float a1, float a2, float a3, float d1, float d2, float d3, float s1, float s2, float s3, float s) const;
    void setLight(float a1, float a2, float a3, float d1, float d2, float d3, float s1, float s2, float s3)const;
private:
    // checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(unsigned int object, std::string type); 
};
  
#endif