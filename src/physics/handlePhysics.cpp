#include "handlePhysics.h"

HandlePhysics::HandlePhysics() {}
HandlePhysics::~HandlePhysics() {}

void HandlePhysics::Update(float deltaTime) {
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