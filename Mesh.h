#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Shader.h"

using namespace std;


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture {
    unsigned int id;
    string type;
    aiString path;  // храним путь к текстуре для нужд сравнения объектов текстур
};

class Mesh {
    public:
        /*  Mesh Data  */
        unsigned int VAO;
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        /*  Functions  */
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
        void Draw(Shader, GLuint = GL_TRIANGLES);
        void BindTextures(Shader shader);
    private:
        /*  Render data  */
        unsigned int VBO, EBO;
        /*  Functions    */
        void setupMesh();
};