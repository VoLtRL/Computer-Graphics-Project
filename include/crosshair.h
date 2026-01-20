#pragma once
#include "shader.h"
#include <glm/glm.hpp>
#include <vector>

class Crosshair {
public:
    Crosshair(float size);
    void draw(unsigned int textureID, float aspectRatio);
private:
    float size;
    GLuint VAO;
    GLuint VBO;
    Shader* shader_program;
};