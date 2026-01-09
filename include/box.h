#pragma once

#include "shape.h"
#include <vector>

class Box : public Shape {
public:
    Box(Shader *shader_program, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;
private:
    unsigned int num_indices;
    GLuint VAO;
    GLuint buffers[2];
};