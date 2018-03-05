#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h> // Подключаем glew для того, чтобы получить все необходимые заголовочные файлы OpenGL

class Shader
{
public:
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    GLuint GetProgram();
    void Use();
private:
    GLuint Program;
    std::string  ReadFile(const GLchar* path);
    GLuint MakeShader(std::string body, GLenum shaderType, const GLchar* vertexPath);
};