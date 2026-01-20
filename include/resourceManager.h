#pragma once

#include <map>
#include <string>
#include "shader.h"

class ResourceManager {
public:
    static std::map<std::string, Shader*> Shaders;
    static std::map<std::string, unsigned int> Textures;

    static Shader* LoadShader(std::string vShaderFile, std::string fShaderFile, std::string name);
    static Shader* GetShader(std::string name);

    static unsigned int LoadTexture(std::string file, std::string name);
    static unsigned int GetTexture(std::string name);

    static void Clear();

private:
    ResourceManager() { }
    static unsigned int loadTextureFromFile(const char* file);
};
