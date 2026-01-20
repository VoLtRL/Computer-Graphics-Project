#include "resourceManager.h"
#include <iostream>
#include <stb_image.h>

// initialize static variables
std::map<std::string, Shader*>       ResourceManager::Shaders;
std::map<std::string, unsigned int>  ResourceManager::Textures;


Shader* ResourceManager::LoadShader(std::string vShaderFile, std::string fShaderFile, std::string name){
    Shaders[name] = new Shader(vShaderFile.c_str(), fShaderFile.c_str());
    return Shaders[name];
}
Shader* ResourceManager::GetShader(std::string name){
    return Shaders[name];
}

unsigned int ResourceManager::LoadTexture(std::string file, std::string name){
    unsigned int textureID = loadTextureFromFile(file.c_str());
    Textures[name] = textureID;
    return textureID;
}

unsigned int ResourceManager::GetTexture(std::string name){
    return Textures[name];
}

void ResourceManager::Clear() {
    for (auto iter : Shaders) glDeleteProgram(iter.second->get_id());
    for (auto iter : Textures) glDeleteTextures(1, &iter.second);
}

unsigned int ResourceManager::loadTextureFromFile(const char* file){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << file << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}