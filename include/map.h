#pragma once
#include <vector>
#include "node.h"
#include "physicShapeObject.h"
#include "box.h"

class Map 
{
    public:
        Map(Shader* shader, Node* sceneRoot);

    private:
        void processNodeRecursive(Node* node, Shader* shader);
};