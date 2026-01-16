#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"

Map::Map(Shader* shader, Node* sceneRoot, std::vector<PhysicObject*>& physicsList) {

    Box* groundShape = new Box(shader, 50.0f, 1.0f, 50.0f); // Large flat box as ground
    PhysicShapeObject* ground = new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f)); // Position the ground at y = -0.5 to align top surface with y = 0
    ground->Mass = 0.0f; // Immovable ground
    ground->canCollide = true; // Enable collisions
    ground->kinematic = false; // Not kinematic


    sceneRoot->add(ground); // Add ground to the scene graph
    physicsList.push_back(ground); // Add ground to the physics simulation list
    
}