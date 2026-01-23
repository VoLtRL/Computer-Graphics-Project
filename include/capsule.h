#pragma once
#include "shape.h"
#include "shader.h"

class Capsule : public Shape {
public:
    Capsule(Shader* shader_program, float radius, float height);
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;
    virtual Shape* clone() const override {
        return new Capsule(*this);
    }
    float radius;
    float height;
private:
    unsigned int num_indices;
    GLuint VAO;
    GLuint buffers[2];
};