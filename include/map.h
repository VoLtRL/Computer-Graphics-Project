#pragma once
#include <vector>
#include "node.h"
#include "physicShapeObject.h"
#include "box.h"

class Map {
public:
    Map(Shader* shader, Node* sceneRoot);

private:
    void CreateCollisionFromNode(
        Node* node,
        Shader* shader,
        Node* sceneRoot,
        const glm::mat4& parentTransform
    );
};