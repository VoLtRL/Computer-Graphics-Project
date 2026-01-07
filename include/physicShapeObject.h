#pragma once

#include "shape.h"
#include "physicObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Shape;

// An instance of a physic object in the scene.
class PhysicShapeObject : public PhysicObject {

public:

	// Constructor
	PhysicShapeObject(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)); // Default : no shape, at origin.

	// The shape representing the object.
	Shape* shape; // May be nullptr.

	// Draw the object using its shape.
	void draw(glm::mat4& view, glm::mat4& projection); // Uses PhysicObject's Position and orientation, doesn't do any physics update.
};