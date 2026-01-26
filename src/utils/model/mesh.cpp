#include "mesh.h"
#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Shader* shader, std::string matName) 
    : Shape(shader) 
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->materialName = matName;
    
    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    
    // tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    glBindVertexArray(0);
}

void Mesh::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    glUseProgram(shader_program_);

    unsigned int modelLoc = glGetUniformLocation(shader_program_, "model");
    unsigned int viewLoc  = glGetUniformLocation(shader_program_, "view");
    unsigned int projLoc  = glGetUniformLocation(shader_program_, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // define material color based on materialName
    glm::vec3 finalColor(0.5f, 0.5f, 0.5f); // gray

    // gold
    if (materialName.find("Trim") != std::string::npos || materialName.find("Gold") != std::string::npos) {
        finalColor = glm::vec3(1.0f, 0.84f, 0.0f); 
    }
    // gray 2
    else if (materialName.find("Steel") != std::string::npos || materialName.find("Metal") != std::string::npos) {
        finalColor = glm::vec3(0.60f, 0.65f, 0.70f); 
    }
    // black
    else if (materialName.find("Dark") != std::string::npos || materialName.find("Leather") != std::string::npos) {
        finalColor = glm::vec3(0.15f, 0.15f, 0.15f); 
    }
    // void
    else if (materialName.find("VOID") != std::string::npos) {
        finalColor = glm::vec3(0.1f, 0.0f, 0.2f); 
    }
    else if (materialName.find("SPECTRAL_T1") != std::string::npos) {
        finalColor = glm::vec3(0.5f, 0.95f, 1.0f);
    }
    else if (materialName.find("SPECTRAL_T2") != std::string::npos) {
        finalColor = glm::vec3(0.85f, 0.50f, 1.0f);
    }
    else if (materialName.find("SPECTRAL_T3") != std::string::npos) {
        finalColor = glm::vec3(1.0f, 0.2f, 0.40f);
    }
    else if (materialName.find("SPECTRAL_T4") != std::string::npos) {
        finalColor = glm::vec3 (0.0f, 0.0f, 0.0f);
    }

    glUniform3fv(glGetUniformLocation(shader_program_, "objectColor"), 1, glm::value_ptr(finalColor));
    glUniform1i(glGetUniformLocation(shader_program_, "useCheckerboard"), 0);
    glUniform1i(glGetUniformLocation(shader_program_, "isEmissive"), 0);

    glUniform1f(glGetUniformLocation(shader_program_, "alpha"), alpha);
    
     glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
     glUniformMatrix3fv(glGetUniformLocation(shader_program_, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    glBindVertexArray(VAO);

    if (alpha < 1.0f) {
        glDepthMask(GL_FALSE); 
        glUniform1i(glGetUniformLocation(shader_program_, "isShadowPass"), false);

    }

    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    if (alpha < 1.0f) {
        glDepthMask(GL_TRUE); 
    }

    glBindVertexArray(0);
}

Shape* Mesh::clone() const {
    return new Mesh(*this);
}