#include "node.h"
#include "shape.h"
#include "physicShapeObject.h"
#include <iostream>

Node::Node(const glm::mat4& transform) :
    transform_(transform) {

        children_ = std::vector<Node*>();
}

void Node::add(Node* node) {
    children_.push_back(node);
}

void Node::add(Shape* shape) {
    children_shape_.push_back(shape);
}

void Node::add(PhysicShapeObject* pso) {
	children_physic_shape_.push_back(pso);  
}

void Node::remove(PhysicShapeObject* pso) {
    auto it = std::find(children_physic_shape_.begin(), children_physic_shape_.end(), pso);
    if (it != children_physic_shape_.end()) {
        children_physic_shape_.erase(it);
    }
}

void Node::draw(glm::mat4& model, glm::mat4& view, glm::mat4& projection) {
    glm::mat4 updatedModel = model * transform_;

    for (auto child : children_) {
        child->draw(updatedModel, view, projection);
    }

    for (auto child : children_shape_) {
        child->draw(updatedModel, view, projection);
    }

    for (auto child : children_physic_shape_) {
        child->draw(view, projection);
    }

}
// Clone the node and its children
Node* Node::clone() const {
    Node* newNode = new Node(this->transform_);
    newNode->name = this->name;

    for (const auto& shape : this->children_shape_) {
        newNode->add(shape->clone());
    }

    for (const auto& pso : this->children_physic_shape_) {
        newNode->add(pso);
    }

    for (const auto& child : this->children_) {
        newNode->add(child->clone());
    }

    return newNode;
}

// Met à jour la matrice de transformation du noeud
void Node::set_transform(const glm::mat4& transform) {
    transform_ = transform;
}

void Node::key_handler(int key) const {
    for (const auto& child : children_) {
            child->key_handler(key);
    }
}

Node::~Node() {
    for (auto child : children_) {
        delete child;
    }
    children_.clear();

    for (auto shape : children_shape_) {
        delete shape;
    }
    children_shape_.clear();

    for (auto pso : children_physic_shape_) {
        delete pso;
    }
    children_physic_shape_.clear();
}

void Node::setAlpha(float alpha) {
    for (auto shape : children_shape_) {
        shape->alpha = alpha;
    }
    for (auto child : children_) {
        child->setAlpha(alpha);
    }
}
