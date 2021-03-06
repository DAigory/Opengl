#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Model.h"
#include "TextureLoader.h"

void Model::Draw(Shader shader, GLuint drawType)
{
    for(unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader, drawType);
}

void Model::loadModel(string path)
{
std::cout << path <<std::endl;
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path,
                                           aiProcess_Triangulate
                                           | aiProcess_FlipUVs
                                           | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // обработать все полигональные сетки в узле(если есть)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // выполнить ту же обработку и для каждого потомка узла
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.Tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.Bitangent = vector;

        // texture coordinates
        if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        vertices.push_back(vertex);
    }
    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // обработка материала
    if(mesh->mMaterialIndex >= 0)
    {
       aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
       vector<Texture> diffuseMaps = loadMaterialTextures(material,
                                           aiTextureType_DIFFUSE, "texture_diffuse");
       textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
       vector<Texture> specularMaps = loadMaterialTextures(material,
                                           aiTextureType_SPECULAR, "texture_specular");
       textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
       vector<Texture> normalMaps = loadMaterialTextures(material,
                                                  aiTextureType_HEIGHT, "texture_normal");
       textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName, GLenum imageFormat)
{
   vector<Texture> textures;
       for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
       {
           aiString str;
           mat->GetTexture(type, i, &str);
           bool skip = false;
           for(unsigned int j = 0; j < textures_loaded.size(); j++)
           {
               if(std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0)
               {
                   textures.push_back(textures_loaded[j]);
                   skip = true;
                   break;
               }
           }
           if(!skip)
           {   // если текстура не была загружена – сделаем это
               Texture texture;
               texture.id = TextureLoader::Load(str.C_Str(), directory, imageFormat);
               texture.type = typeName;
               texture.path = str;
               textures.push_back(texture);
   // занесем текстуру в список уже загруженных
               textures_loaded.push_back(texture);
               //cout << this->directory + " load "+ texture.type << endl;
           }
       }
       return textures;
}