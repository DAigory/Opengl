#include "Shader.h"

void Shader::Use(){
        glUseProgram(this->Program);
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
        GLuint vertex, fragment;
        vertex = this->MakeShader(this->ReadFile(vertexPath), GL_VERTEX_SHADER, vertexPath);
        fragment = this->MakeShader(this->ReadFile(fragmentPath), GL_FRAGMENT_SHADER, fragmentPath);
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        this->LinkProgram();
        glDeleteShader(vertex);
        glDeleteShader(fragment);
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
    :Shader(vertexPath, fragmentPath)
{
    GLuint geometry;
    geometry = this->MakeShader(this->ReadFile(geometryPath), GL_GEOMETRY_SHADER, geometryPath);
    glAttachShader(this->Program, geometry);
    this->LinkProgram();
    glDeleteShader(geometry);
}

GLuint Shader::GetId(){
    return this->Program;
}

void Shader::LinkProgram(){
        glLinkProgram(this->Program);
        GLint success;
        GLchar infoLog[512];
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
}

GLuint Shader::MakeShader(std::string body, GLenum shaderType, const GLchar* path){
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
            std::cout << "ERROR::SHADER::"<< shaderTypeText << "::COMPILATION_FAILED\n" << infoLog << "file: "<< path << std::endl;

        }
        return shader;
}

void Shader::SetValue(const GLchar* name, int value){
     glUniform1i(this->GetUniformId(name), value);
}
void Shader::SetValue(const GLchar* name, float a, float b, float c){
     glUniform3f(this->GetUniformId(name), a, b, c);
}
void Shader::SetValue(const GLchar* name, glm::vec3 value){
    this->SetValue(name, value.x, value.y, value.z);
}
void Shader::SetValue(const GLchar* name, float a, float b, float c, float d){
     glUniform4f(this->GetUniformId(name), a, b, c, d);
}
void Shader::SetValue(const GLchar* name, glm::vec4 value){
    this->SetValue(name, value.x, value.y, value.z, value.w);
}
void Shader::SetValue(const GLchar* name, float value){
     glUniform1f(this->GetUniformId(name), value);
}

void Shader::SetValue(const GLchar* name, glm::mat4 value){
     glUniformMatrix4fv(this->GetUniformId(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetValue(const GLchar* name, glm::mat3 value){
     glUniformMatrix3fv(this->GetUniformId(name), 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::GetUniformId(const GLchar* name){
    return glGetUniformLocation(this->GetId(), name);
}

GLuint Shader::GetUniformBlockId(const GLchar* name){
    return glGetUniformBlockIndex(this->GetId(), name);
}

void Shader::BindUniformBlock(const GLchar* uniformBlockName, int pointBind){
    unsigned int uniformBlockId = this->GetUniformBlockId(uniformBlockName);
    glUniformBlockBinding(this->GetId(), uniformBlockId, pointBind);
}

std::string  Shader::ReadFile(const GLchar* path){
     std::string code;
     std::ifstream ShaderFile;

     // ensures ifstream objects can throw exceptions:
     ShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
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
         std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ fileName:" << path << std::endl;
     }
     return code;
}
