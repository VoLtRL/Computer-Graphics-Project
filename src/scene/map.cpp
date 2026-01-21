#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"

Map::Map(Shader* shader, Node* sceneRoot) {
    Box* groundShape = new Box(shader, 50.0f, 1.0f, 50.0f); // Large flat box as ground
    PhysicShapeObject* ground = new PhysicShapeObject(groundShape, glm::vec3(0.0f, -0.5f, 0.0f)); // Position the ground at y = -0.5 to align top surface with y = 0
    groundShape->color = glm::vec3(1.0f, 1.0f, 1.0f); 
    groundShape->useCheckerboard = true; // Enable checkerboard pattern
    ground->SetMass(0.0f); // Immovable ground
    ground->canCollide = true; // Enable collisions
    ground->kinematic = false; // Not kinematic
	ground->name = "Ground";
	ground->shapeType = BOX;
	ground->Damping = 1.0f; // No damping
    ground->Friction = 0.5f;
	ground->collisionShape = groundShape;
    ground->Restitution = 1.0f;
    ground->collisionGroup = CG_ENVIRONMENT;
    ground->collisionMask = CG_PLAYER | CG_ENEMY | CG_PLAYER_PROJECTILE | CG_ENEMY_PROJECTILE | CG_ENVIRONMENT;


    sceneRoot->add(ground); // Add ground to the scene graph
    
}