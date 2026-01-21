#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shape.h"
#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh : public Shape {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;
    
    std::string materialName; 

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Shader* shader, std::string matName) 
        : Shape(shader) 
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->materialName = matName;

        //To delete?
        this->useCheckerboard = false;
        
        setupMesh();
    }

    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override {
        glUseProgram(shader_program_);

        unsigned int modelLoc = glGetUniformLocation(shader_program_, "model");
        unsigned int viewLoc  = glGetUniformLocation(shader_program_, "view");
        unsigned int projLoc  = glGetUniformLocation(shader_program_, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // --- GESTION DES COULEURS SELON LE NOM DU MATÉRIAU ---
        glm::vec3 finalColor(0.5f, 0.5f, 0.5f); // Gris par défaut

        // Si le nom contient "TRIM" ou "GOLD" -> COULEUR OR
        if (materialName.find("Trim") != std::string::npos || materialName.find("Gold") != std::string::npos) {
            finalColor = glm::vec3(1.0f, 0.84f, 0.0f); // Or Jaune
        }
        // Si le nom contient "STEEL" ou "METAL" -> GRIS ACIER BLEUTÉ
        else if (materialName.find("Steel") != std::string::npos || materialName.find("Metal") != std::string::npos) {
            finalColor = glm::vec3(0.60f, 0.65f, 0.70f); // Gris Acier
        }
        // Si le nom contient "DARK" ou "LEATHER" -> NOIR ANTHRACITE
        else if (materialName.find("Dark") != std::string::npos || materialName.find("Leather") != std::string::npos) {
            finalColor = glm::vec3(0.15f, 0.15f, 0.15f); // Gris très foncé
        }

        glUniform3fv(glGetUniformLocation(shader_program_, "objectColor"), 1, glm::value_ptr(finalColor));
        glUniform1i(glGetUniformLocation(shader_program_, "useCheckerboard"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    unsigned int VBO, EBO;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        glBindVertexArray(0);
    }
};