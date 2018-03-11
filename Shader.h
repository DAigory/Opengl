#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    GLuint GetProgram();
    void SetValue(const GLchar* name, int value);
    void SetValue(const GLchar* name, float a, float b, float c);
    void SetValue(const GLchar* name, glm::vec3 value);
    void SetValue(const GLchar* name, float value);
    void SetValue(const GLchar* name, glm::vec4 value);
    void SetValue(const GLchar* name, float a, float b, float c, float d);
    void SetValue(const GLchar* name, glm::mat4 value);
    void SetValue(const GLchar* name, glm::mat3 value);

    void Use();
private:
    GLuint Program;
    std::string  ReadFile(const GLchar* path);
    GLuint MakeShader(std::string body, GLenum shaderType, const GLchar* vertexPath);
    GLuint GetUniformId(const GLchar* name);
};