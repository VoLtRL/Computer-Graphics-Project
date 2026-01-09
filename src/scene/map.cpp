#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"

Map::Map(Shader* shader, Node* sceneRoot, std::vector<PhysicObject*>& physicsList) {

    Box* groundShape = new Box(shader, 50.0f, 1.0f, 50.0f);
    PhysicShapeObject* ground = new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f));
    ground->Mass = 0.0f; // Immovable ground
    ground->canCollide = true; // Enable collisions
    ground->kinematic = false; // Not kinematic


    sceneRoot->add(ground);
    physicsList.push_back(ground);
    
}