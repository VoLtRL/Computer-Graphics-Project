#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shape.h"
#include "shader.h"

// Structure de données
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
    // Données du maillage
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;
    std::string materialName; 

    // Constructeur
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, Shader* shader, std::string matName);

    // Fonction de dessin (Override de Shape)
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) override;

private:
    // Buffers OpenGL
    unsigned int VBO, EBO;

    // Initialisation des buffers
    void setupMesh();
};