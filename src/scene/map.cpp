#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"

#include "model.h"  
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>

Map::Map(Shader* shader, Node* sceneRoot) {
    std::string mapPath = IMAGE_DIR + std::string("map.glb");

    Box* groundShape = new Box(shader, 50.0f, 1.0f, 50.0f); // Large flat box as ground
    PhysicShapeObject* ground = new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f)); // Position the ground at y = -0.5 to align top surface with y = 0
    groundShape->color = glm::vec3(1.0f, 1.0f, 1.0f); 
    groundShape->useCheckerboard = true; // Enable checkerboard pattern
    ground->SetMass(0.0f); // Immovable ground
    ground->kinematic = false; // Not kinematic
	ground->name = "Ground";
	ground->shapeType = ShapeType::ST_BOX;
	ground->Damping = 1.0f; // No damping
    ground->Friction = 0.5f;
	ground->collisionShape = groundShape;
    ground->Restitution = 1.0f;
    ground->collisionGroup = CG_ENVIRONMENT;
    ground->collisionMask = CG_PRESETS_MAP;

    sceneRoot->add(ground); // Add ground to the scene graph

    // Load the map model from a .glb file
    Model* gameMap = new Model(mapPath, shader);

    // Y = -2 to align the map correctly above the ground
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, -2.0f, 0.0f));

    // Added to scene
    if (gameMap->rootNode) {
        gameMap->rootNode->set_transform(transform);
        gameMap->rootNode->name = "Map_Projet";
        sceneRoot->add(gameMap->rootNode);
    }
}
