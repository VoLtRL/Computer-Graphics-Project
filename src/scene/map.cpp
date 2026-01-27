#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"
#include "model.h"  
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>

<<<<<<< HEAD
#include "model.h"  
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>

Map::Map(Shader* shader, Node* sceneRoot)
{
    std::string mapPath = IMAGE_DIR + std::string("map.glb");

    Box* groundShape = new Box(shader, 50.0f, 1.0f, 50.0f);
    PhysicShapeObject* ground =
        new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f));

    groundShape->useCheckerboard = true;
    ground->SetMass(0.0f);
    ground->kinematic = false;
    ground->name = "Ground";
    ground->shapeType = ShapeType::ST_BOX;
    ground->collisionShape = groundShape;
=======
Map::Map(Shader* shader, Node* sceneRoot)
{
    std::string mapPath = IMAGE_DIR + std::string("map.glb");
    Box* groundShape = new Box(shader, 100.0f, 1.0f, 100.0f); // Large flat box as ground
    PhysicShapeObject* ground = new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f)); // Position the ground at y = -0.5 to align top surface with y = 0
    groundShape->color = glm::vec3(1.0f, 1.0f, 1.0f); 
    groundShape->useCheckerboard = true; // Enable checkerboard pattern
    ground->SetMass(0.0f); // Immovable ground
    ground->kinematic = false; // Not kinematic
	ground->name = "Ground";
	ground->Damping = 1.0f; // No damping
    ground->Friction = 0.5f;
	ground->collisionShape = groundShape;
    ground->Restitution = 1.0f;
>>>>>>> main
    ground->collisionGroup = CG_ENVIRONMENT;
    ground->collisionMask = CG_PRESETS_MAP;

    sceneRoot->add(ground);
<<<<<<< HEAD

    Model* gameMap = new Model(mapPath, shader);

    if (!gameMap || !gameMap->rootNode)
        return;

    glm::mat4 transform = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, -2.0f, 0.0f)
    );

    Node* root = gameMap->rootNode;
    root->set_transform(transform);
    root->name = "Map_Projet";

    if (gameMap->rootNode)
    {
        gameMap->rootNode->set_transform(transform);
        gameMap->rootNode->name = "Map_Projet";

        processNodeRecursive(gameMap->rootNode, shader);

        sceneRoot->add(gameMap->rootNode);
    }
}


void Map::processNodeRecursive(Node* node, Shader* shader)
{
    if (!node) return;

    const std::string& name = node->name;


    if (name.find("COLLIDER_BOX") != std::string::npos)
    {
        Box* box = new Box(shader, 1.0f, 1.0f, 1.0f);
        PhysicShapeObject* collider = new PhysicShapeObject(box, glm::vec3(0.0f, -0.5f, 0.0f));
        collider->SetMass(0.0f);
        collider->kinematic = false;
        collider->name = "Ground";
        collider->shapeType = ShapeType::ST_BOX;
        collider->collisionShape = box;
        collider->collisionGroup = CG_ENVIRONMENT;
        collider->collisionMask = CG_PRESETS_MAP;
    }

    for (Node* child : node->getChildren())
    {
        processNodeRecursive(child, shader);
    }
}

=======

}
>>>>>>> main
