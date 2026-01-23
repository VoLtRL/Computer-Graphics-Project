#pragma once

#include "shader.h"
#include "node.h"

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Shape {
public:
    Shape(Shader *shader_program);

    virtual void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

    virtual ~Shape() = default;
    glm::vec3 color;
    bool useCheckerboard;
    bool isEmissive;

    virtual Shape* clone() const{
        return new Shape(*this);
    }

protected:
    GLuint shader_program_;
};
