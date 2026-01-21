#include "box.h"

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

Box::Box(Shader *shader_program, float width, float height, float depth)
    : Shape(shader_program)
{
    // half dimensions to center the box at origin
    w = width * 0.5f;
    h = height * 0.5f;
    d = depth * 0.5f;

    // define vertices
    std::vector<glm::vec3> vertices = {
        // Front face
        {-w, -h,  d}, { w, -h,  d}, { w,  h,  d}, {-w,  h,  d},
        // Back face
        {-w, -h, -d}, {-w,  h, -d}, { w,  h, -d}, { w, -h, -d},
        // Left face
        {-w, -h, -d}, {-w, -h,  d}, {-w,  h,  d}, {-w,  h, -d},
        // Right face
        { w, -h, -d}, { w,  h, -d}, { w,  h,  d}, { w, -h,  d},
        // Top face
        {-w,  h, -d}, {-w,  h,  d}, { w,  h,  d}, { w,  h, -d},
        // Bottom face
        {-w, -h, -d}, { w, -h, -d}, { w, -h,  d}, {-w, -h,  d},
    };

    // define indices
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9,10,10,11, 8,
       12,13,14,14,15,12,
       16,17,18,18,19,16,
       20,21,22,22,23,20
    };

    // setup OpenGL buffers and arrays
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(2, &buffers[0]);

    // create vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // create index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    num_indices = static_cast<unsigned int>(indices.size());
}

void Box::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
    // bind shader and vertex array
    glUseProgram(this->shader_program_);
    glBindVertexArray(VAO);

    // call parent draw to set uniforms
    Shape::draw(model, view, projection);

    // draw elements
    glDrawElements(GL_TRIANGLE_STRIP, num_indices, GL_UNSIGNED_INT, nullptr);
}
