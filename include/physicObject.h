#pragma once

#include <../external/glm/glm/glm.hpp>
#include <../external/glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

class physicShapeObject; // Forward declaration

class PhysicObject {

public:
	PhysicObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));

	// Position and movement
	glm::vec3 Position;			// The object's world position.
	glm::vec3 Velocity;			// The object's velocity, updates position.
	glm::vec3 Acceleration;		// The object's acceleration, updates velocity.
	float Damping;				// Damping strength, updates velocity.

	// Orientation vectors
	glm::vec3 RightVector;		// The object's rightward vector.
	glm::vec3 UpVector;			// The object's upward vector.
	glm::vec3 FrontVector;		// The object's forward vector.
	const glm::vec3 WorldUpVector;	// The world's upward vector. TO BE MOVED TO A GLOBAL CONSTANT

	// Mass
	float Mass;					// The object's mass in kg.
	bool kinematic;				// True if the object is kinematic (not affected by forces).
	const float gravity;		// Gravitational acceleration in m*s^(-2). TO BE MOVED TO A GLOBAL CONSTANT

	// Collisions
	bool canCollide;			// True if the object can collide with other collidable objects.
	glm::vec3 forcesApplied;	// Instant forces in Newton, updates acceleration. Does not include damping, gravity or inner accelerations.

	// Update physics state
	void UpdatePhysics(float deltaTime);

};