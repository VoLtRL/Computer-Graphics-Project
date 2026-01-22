#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

Model::Model(std::string const &path, Shader* shader) : shader(shader) {
    loadModel(path);
}

void Model::Draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    if(rootNode)
        rootNode->draw(model, view, projection);
}

void Model::loadModel(std::string const &path) {
    Assimp::Importer importer;
    // Options de post-traitement : Triangulate, FlipUVs, CalcTangentSpace
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    rootNode = processNode(scene->mRootNode, scene);
}

Node* Model::processNode(aiNode *node, const aiScene *scene) {
    Node* newNode = new Node();
    newNode->name = node->mName.C_Str();
    
    // Conversion de la transformation
    glm::mat4 transform = aiMatrix4x4ToGlm(node->mTransformation);
    newNode->set_transform(transform);

    // Traitement des meshes du noeud
    for(unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        Mesh* myMesh = processMesh(mesh, scene);
        newNode->add(myMesh);
    }

    // Traitement des enfants
    for(unsigned int i = 0; i < node->mNumChildren; i++) {
        Node* childNode = processNode(node->mChildren[i], scene);
        newNode->add(childNode);
    }

    return newNode;
}

Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // 1. Récupération des vertices
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // Positions
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        // Normales
        if (mesh->HasNormals())
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        
        // Coordonnées de texture et Tangentes
        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            if (mesh->HasTangentsAndBitangents())
                vertex.Tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // 2. Récupération des indices (faces)
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 3. Récupération du nom du matériau
    std::string matName = "default";
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        aiString str;
        if(material->Get(AI_MATKEY_NAME, str) == AI_SUCCESS) {
            matName = str.C_Str();
        }
    }

    return new Mesh(vertices, indices, textures, shader, matName);
}

glm::mat4 Model::aiMatrix4x4ToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    // Assimp est Row-Major, GLM est Column-Major, donc on transpose lors de la copie
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}