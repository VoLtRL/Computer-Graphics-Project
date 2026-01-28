#pragma once
#include <vector>
#include "physicObject.h"

class Node;

class HandlePhysics {
public:
    HandlePhysics(Node* root);
    ~HandlePhysics();
    Node* root;
    void Update(float deltaTime);

    const std::vector<PhysicObject*>& GetObjects() const { return PhysicObject::allPhysicObjects ;};


};