#pragma once

#include <map>
#include <string>
#include "shader.h"
#include "model.h"

class ResourceManager {
public:
    static std::map<std::string, Shader*> Shaders;
    static std::map<std::string, unsigned int> Textures;
    static std::map<std::string, Model*> Models;

    static Shader* LoadShader(std::string vShaderFile, std::string fShaderFile, std::string name);
    static Shader* GetShader(std::string name);

    static Model* LoadModel(std::string file, std::string name, Shader* shader);
    static Model* GetModel(std::string name);

    static unsigned int LoadTexture(std::string file, std::string name);
    static unsigned int GetTexture(std::string name);

    static void Clear();

private:
    ResourceManager() { }
    static unsigned int loadTextureFromFile(const char* file);
};
