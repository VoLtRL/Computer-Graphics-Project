#include "cylinder.h"

#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>

Cylinder::Cylinder(Shader *shader_program, float height, float radius, int slices)
    : Shape(shader_program)
{
    std::vector<float> vertices;
    
    for (int i = 0; i < slices; i++) {
        float theta = 2.0f * glm::pi<float>() * static_cast<float>(i) / static_cast<float>(slices);
        float x = radius * glm::cos(theta);
        float y = radius * glm::sin(theta);
        
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.5f * height);
        vertices.push_back(x / radius);
        vertices.push_back(y / radius);
        vertices.push_back(0.0f);

        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(-0.5f * height);
        vertices.push_back(x / radius);
        vertices.push_back(y / radius);
        vertices.push_back(0.0f);
    }

    vertices.push_back(0.0f); 
    vertices.push_back(0.0f); 
    vertices.push_back(0.5f * height);
    vertices.push_back(0.0f); 
    vertices.push_back(0.0f);
     vertices.push_back(1.0f);

    vertices.push_back(0.0f); 
    vertices.push_back(0.0f); 
    vertices.push_back(-0.5f * height);
    vertices.push_back(0.0f); 
    vertices.push_back(0.0f); 
    vertices.push_back(-1.0f);

    std::vector<unsigned int> indices;
    for (int i = 0; i < slices; i++) {
        indices.push_back(2 * i);
        indices.push_back(2 * i + 1);
        indices.push_back((2 * i + 2) % (2 * slices));
        indices.push_back(2 * i + 1);
        indices.push_back((2 * i + 3) % (2 * slices));
        indices.push_back((2 * i + 2) % (2 * slices));
        
        indices.push_back(2 * i);
        indices.push_back((2 * i + 2) % (2 * slices));
        indices.push_back(vertices.size()/6 - 2);
        
        indices.push_back(2 * i + 1);
        indices.push_back(vertices.size()/6 - 1);
        indices.push_back((2 * i + 3) % (2 * slices));
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(2, &buffers[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    num_indices = static_cast<unsigned int>(indices.size());
}

void Cylinder::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection)
{
    glUseProgram(this->shader_program_);
    glBindVertexArray(VAO);

    Shape::draw(model, view, projection);

    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, nullptr);
}
