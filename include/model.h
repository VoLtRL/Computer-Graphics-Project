#pragma once

#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include <assimp/scene.h>

#include "mesh.h"
#include "shader.h"
#include "node.h"

class Model {
public:
    Node* rootNode;

    // builder
    Model(std::string const &path, Shader* shader);

    void Draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);

    Model* clone(Shader* shader);

private:
    Shader* shader;
    std::string directory;

    void loadModel(std::string const &path);
    Node* processNode(aiNode *node, const aiScene *scene);
    Mesh* processMesh(aiMesh *mesh, const aiScene *scene);
    
    glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from);
};