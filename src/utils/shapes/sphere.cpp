#include "sphere.h"


Sphere::Sphere(Shader* shader_program, float radius, int slices) : Shape(shader_program){
    // generate vertices
	radius = radius;
    std::vector<glm::vec3> vertices;
    for (int i = 0; i <= slices; i++) {
        float theta = glm::pi<float>() * static_cast<float>(i) / static_cast<float>(slices);
        for (int j = 0; j <= slices; j++) {
            float phi = 2.0f * glm::pi<float>() * static_cast<float>(j) / static_cast<float>(slices);
            float x = radius * glm::sin(theta) * glm::cos(phi);
            float y = radius * glm::sin(theta) * glm::sin(phi);
            float z = radius * glm::cos(theta);
            vertices.push_back(glm::vec3(x, y, z));
        
    }
    }
    // generate indices
    std::vector<unsigned int> indices;
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < slices; j++) {
            indices.push_back(i * (slices + 1) + j);
            indices.push_back((i + 1) * (slices + 1) + j);
            indices.push_back((i + 1) * (slices + 1) + (j + 1));
            indices.push_back(i * (slices + 1) + j);
            indices.push_back((i + 1) * (slices + 1) + (j + 1));
            indices.push_back(i * (slices + 1) + (j + 1));
        }
    }
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(2, &buffers[0]);
    // create vertex buffer
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    // create index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    num_indices = static_cast<unsigned int>(indices.size());
}   

void Sphere::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection){

    glUseProgram(this->shader_program_);
    glBindVertexArray(VAO);

    Shape::draw(model, view, projection);

    /* draw points 0-3 from the currently bound VAO with current in-use shader */
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);
}

Sphere::~Sphere(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, &buffers[0]);
}

