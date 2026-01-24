#pragma once
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Sprite {
public:
    Sprite(Shader* shader);
    ~Sprite();
    void draw(unsigned int textureID, glm::vec2 position, glm::vec2 size, float rotate = 0.0f, glm::vec3 color = glm::vec3(1.0f));
    
    static glm::mat4 getProjection(float aspectRatio);

private:
    Shader* shader;
    unsigned int quadVAO;
    void initRenderData();
};