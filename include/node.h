#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "shape.h"
#include "physicShapeObject.h"

class Shape;
class PhysicShapeObject;

class Node {
public:
    Node(const glm::mat4& transform = glm::mat4(1.0f));
    const std::vector<Node*>& getChildren() const;
    void add(Node* node);
    void add(Shape* shape);
    void add(PhysicShapeObject* pso);
    void draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection);
    void set_transform(const glm::mat4 &transform); // sets local transform
    std::vector<Node *> children_;
    glm::mat4 get_transform() { return transform_; };
    std::string name;
    void key_handler(int key) const;
    Node* clone() const;
    ~Node();

private:
    glm::mat4 transform_;
    std::vector<Shape *> children_shape_;
	std::vector<PhysicShapeObject *> children_physic_shape_;

};
