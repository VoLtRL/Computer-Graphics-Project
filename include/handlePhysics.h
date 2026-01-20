#pragma once
#include <vector>
#include "physicObject.h"

class HandlePhysics {
public:
    HandlePhysics();
    ~HandlePhysics();

    void AddObject(PhysicObject* obj);
    void Update(float deltaTime);

    const std::vector<PhysicObject*>& GetObjects() const { return objects; }

private:
    std::vector<PhysicObject*> objects;
};