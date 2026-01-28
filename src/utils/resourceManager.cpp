#include "resourceManager.h"
#include <iostream>
#include <stb_image.h>

// initialize static variables
std::map<std::string, Shader*>       ResourceManager::Shaders;
std::map<std::string, unsigned int>  ResourceManager::Textures;
std::map<std::string, Model*>        ResourceManager::Models;


Shader* ResourceManager::LoadShader(std::string vShaderFile, std::string fShaderFile, std::string name){
    Shaders[name] = new Shader(vShaderFile.c_str(), fShaderFile.c_str());
    return Shaders[name];
}
Shader* ResourceManager::GetShader(std::string name){
    return Shaders[name];
}

unsigned int ResourceManager::LoadTexture(std::string file, std::string name){
    stbi_set_flip_vertically_on_load(true);
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
    for (auto iter : Models) delete iter.second;
}

unsigned int ResourceManager::loadTextureFromFile(const char* file){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;

    unsigned char* data = stbi_load(file, &width, &height, &nrComponents, 4);
    if (data){
        GLenum format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        /* std::cout << "Texture failed to load at path: " << file << std::endl; */
        stbi_image_free(data);
    }
    return textureID;
}

Model* ResourceManager::LoadModel(std::string file, std::string name, Shader* shader) {
    Models[name] = new Model(file, shader);
    return Models[name];
}

Model* ResourceManager::GetModel(std::string name) {
    return Models[name];
}