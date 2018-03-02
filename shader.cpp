#include "shader.h"

void Shader::Use(){
        glUseProgram(this->Program);
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
        GLuint vertex, fragment;
        vertex = this->MakeShader(this->ReadFile("shaders/shader.v"), GL_VERTEX_SHADER);
        fragment = this->MakeShader(this->ReadFile("shaders/shader.f"), GL_FRAGMENT_SHADER);
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
        glDeleteShader(vertex);
        glDeleteShader(fragment);
}

GLuint Shader::GetProgram(){
    return this->Program;
}

GLuint Shader::MakeShader(std::string body, GLenum shaderType){
        GLuint shader;
        GLint success;
        GLchar infoLog[512];
        // Vertex Shader
        shader = glCreateShader(shaderType);
        const GLchar* body_char = body.c_str();
        glShaderSource(shader, 1, &body_char, NULL);
        glCompileShader(shader);
        // Print compile errors if any
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            auto shaderTypeText = "undefinedShaiderType";
            if(shaderType == GL_VERTEX_SHADER){
                shaderTypeText = "vertex";
            }
            if(shaderType == GL_FRAGMENT_SHADER){
                shaderTypeText = "fragment";
            }
            std::cout << "ERROR::SHADER::"<< shaderTypeText << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
        return shader;
}

std::string  Shader::ReadFile(const GLchar* path){
     std::string code;
     std::ifstream ShaderFile;

     // ensures ifstream objects can throw exceptions:
     ShaderFile.exceptions (std::ifstream::badbit);
     try
     {
         // Open files
         ShaderFile.open(path);
         std::stringstream ShaderStream;
         // Read file's buffer contents into streams
         ShaderStream << ShaderFile.rdbuf();
         // close file handlers
         ShaderFile.close();
         // Convert stream into string
         code = ShaderStream.str();
     }
     catch (std::ifstream::failure e)
     {
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << path << std::endl;
     }
     return code;
}
