#include "physicShapeObject.h"
#include "shape.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

PhysicShapeObject::PhysicShapeObject(Shape* newShape, glm::vec3 position)
    : PhysicObject(position), shape(newShape)
{
	// Nothing else to do here
}

PhysicShapeObject::~PhysicShapeObject() {
    if (shape) {
        delete shape;
        shape = nullptr;
    }
}

void PhysicShapeObject::draw(glm::mat4& view,glm::mat4& projection)
{
    if (!shape) {
        // std::cerr << "Warning: PhysicShapeObject has no shape assigned!\n";
        return;
    }

    // Create a model matrix based on PhysicObject's Position
    glm::mat4 model = glm::mat4(1.0f); // Identity
    model = glm::translate(model, Position); // Move to object's position

	// Create rotation matrix from orientation vectors

	model *= RotationMatrix; // Apply rotation

    // Draw the shape with model/view/projection
    shape->draw(model, view, projection);
}
