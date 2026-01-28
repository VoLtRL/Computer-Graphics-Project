#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"
#include "model.h"  
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cfloat>
#include "mesh.h"

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

static AABB ComputeMeshAABB(Mesh* mesh) {
    AABB box;
    box.min = glm::vec3(FLT_MAX);
    box.max = glm::vec3(-FLT_MAX);

    for (const Vertex& v : mesh->vertices) {
        box.min = glm::min(box.min, v.Position);
        box.max = glm::max(box.max, v.Position);
    }
    return box;
}


Map::Map(Shader* shader, Node* sceneRoot)
{
    std::string visualPath = IMAGE_DIR + std::string("map_projet_visuel.glb"); 
    std::string collisionPath = IMAGE_DIR + std::string("map_projet_collisions.glb");
    Model* visualMap = new Model(visualPath, shader);
    sceneRoot->add(visualMap->rootNode);

    Model* collisionMap = new Model(collisionPath, shader);
    CreateCollisionFromNode(
        collisionMap->rootNode,
        shader,
        sceneRoot,
        glm::mat4(1.0f)
    );
}


void Map::CreateCollisionFromNode(
    Node* node,
    Shader* shader,
    Node* sceneRoot,
    const glm::mat4& parentTransform
) {
    glm::mat4 globalTransform = parentTransform * node->get_transform();

    for (Shape* shape : node->getShapes()) {
        Mesh* mesh = dynamic_cast<Mesh*>(shape);
        if (!mesh) continue;

        AABB aabb = ComputeMeshAABB(mesh);

        glm::vec3 size = aabb.max - aabb.min;
        glm::vec3 center = (aabb.min + aabb.max) * 0.5f;
        glm::vec3 worldCenter =
            glm::vec3(globalTransform * glm::vec4(center, 1.0f));

        Box* collisionBox = new Box(shader, size.x, size.y, size.z);

        PhysicShapeObject* phys =
            new PhysicShapeObject(collisionBox, worldCenter);

        phys->SetMass(0.0f);
        phys->kinematic = false;
        phys->collisionShape = collisionBox;
        phys->collisionGroup = CG_ENVIRONMENT;
        phys->collisionMask = CG_PRESETS_MAP;
        phys->name = node->name;

    }

    for (Node* child : node->getChildren()) {
        CreateCollisionFromNode(child, shader, sceneRoot, globalTransform);
    }
}