#include "capsule.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

Capsule::Capsule(Shader* shader_program, float radius, float height)
    : Shape(shader_program), radius(radius), height(height) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const unsigned int segments = 20;
    const unsigned int rings = 10;

    // cylinder part
    for (unsigned int i = 0; i <= segments; ++i) {
        float theta = i * 2.0f * M_PI / segments;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        for (unsigned int j = 0; j <= rings; ++j) {
            float y = -height / 2 + j * (height / rings);
            vertices.push_back(radius * cosTheta);
            vertices.push_back(y);
            vertices.push_back(radius * sinTheta); 
        }
    }

    // cylinder indices
    for (unsigned int i = 0; i < segments; ++i) {
        for (unsigned int j = 0; j < rings; ++j) {
            unsigned int first = (i * (rings + 1)) + j;
            unsigned int second = first + rings + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    // top hemisphere part
    for (unsigned int i = 0; i <= segments; ++i) {
        float theta = i * 2.0f * M_PI / segments;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta); 
        for (unsigned int j = 0; j <= rings; ++j) {
            float phi = j * M_PI / (2 * rings);
            float x = radius * sin(phi) * cosTheta;
            float y = radius * cos(phi) + height / 2;
            float z = radius * sin(phi) * sinTheta;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    // top hemisphere indices
    for (unsigned int i = 0; i < segments; ++i) {
        for (unsigned int j = 0; j < rings; ++j) {
            unsigned int first = (segments + 1) * (rings + 1) + (i * (rings + 1)) + j;
            unsigned int second = first + rings + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    // bottom hemisphere part
    for (unsigned int i = 0; i <= segments; ++i) {
        float theta = i * 2.0f * M_PI / segments;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta); 
        for (unsigned int j = 0; j <= rings; ++j) {
            float phi = j * M_PI / (2 * rings);
            float x = radius * sin(phi) * cosTheta;
            float y = -radius * cos(phi) - height / 2;
            float z = radius * sin(phi) * sinTheta;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    // bottom hemisphere indices
    for (unsigned int i = 0; i < segments; ++i) {
        for (unsigned int j = 0; j < rings; ++j) {
            unsigned int first = 2 * (segments + 1) * (rings + 1) + (i * (rings + 1)) + j;
            unsigned int second = first + rings + 1;
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    // setup OpenGL buffers
    num_indices = indices.size();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, buffers);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}   

void Capsule::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    Shape::draw(model, view, projection);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
