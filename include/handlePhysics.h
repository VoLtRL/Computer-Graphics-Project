#pragma once
#include <vector>
#include "physicObject.h"

class HandlePhysics {
public:
    HandlePhysics();
    ~HandlePhysics();

    void Update(float deltaTime);

    const std::vector<PhysicObject*>& GetObjects() const { return PhysicObject::allPhysicObjects ;};

};