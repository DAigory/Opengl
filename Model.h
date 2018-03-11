#include "Mesh.h"

using namespace std;

class Model
{
    public:
        /*  Методы   */
        Model(char *path)
        {
            loadModel(path);
        }
        void Draw(Shader shader);
    private:
        /*  Данные модели  */
        vector<Mesh> meshes;
        string directory;
        vector<Texture> textures_loaded;
        /*  Методы   */
        void loadModel(string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};