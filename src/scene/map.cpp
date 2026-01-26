#include "map.h"    
#include "box.h"
#include "physicShapeObject.h"
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
    ground->collisionShape = groundShape;
    ground->collisionGroup = CG_ENVIRONMENT;
    ground->collisionMask = CG_PRESETS_MAP;

    sceneRoot->add(ground);

}