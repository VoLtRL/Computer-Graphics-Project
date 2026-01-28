#include "handlePhysics.h"
#include "node.h"

HandlePhysics::HandlePhysics(Node* root) : root(root){}
HandlePhysics::~HandlePhysics() {}

void HandlePhysics::Update(float deltaTime) {
	
    auto it = PhysicObject::physicObjectsToDelete.begin();
    while (PhysicObject::physicObjectsToDelete.size() > 0 &&  it != PhysicObject::physicObjectsToDelete.end()) {
        PhysicObject* po = *it;

        PhysicObject::physicObjectsToDelete.erase(it);
		
		PhysicShapeObject* pso = dynamic_cast<PhysicShapeObject*>(po);
        if (pso) {
            root->recursiveRemove(pso);
        }
        delete po;
		it = PhysicObject::physicObjectsToDelete.begin();
    }


    for (auto obj : PhysicObject::allPhysicObjects) {
        obj->UpdatePhysics(deltaTime);
    }

    int n = PhysicObject::allPhysicObjects.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            PhysicObject* objA = PhysicObject::allPhysicObjects[i];
            PhysicObject* objB = PhysicObject::allPhysicObjects[j];

            CollisionInfo info = PhysicObject::checkCollision(objA, objB);
            if (info.hit) {
                PhysicObject::ResolveCollision(objA, objB, info, deltaTime);
            }
        }
    }
}