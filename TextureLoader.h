#pragma once

#include <SOIL.h>
using namespace std;

class TextureLoader{
public:
    static GLuint Load(const char *path, const string &directory){
        string filename = string(path);
        filename = directory + '/' + filename;
        int width_img, height_img;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width_img, &height_img, 0, SOIL_LOAD_RGB);
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        char test = image[0];

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_img, height_img, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        SOIL_free_image_data(image);
        glBindTexture(GL_TEXTURE_2D, 0);
        return  texture;
    }
};