#include "handlePhysics.h"

HandlePhysics::HandlePhysics() {}
HandlePhysics::~HandlePhysics() {}

void HandlePhysics::AddObject(PhysicObject* obj) {
    objects.push_back(obj);
}

void HandlePhysics::Update(float deltaTime) {
    for (auto obj : objects) {
        obj->UpdatePhysics(deltaTime);
    }
}