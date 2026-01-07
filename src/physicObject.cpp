#include "physicObject.h"

PhysicObject::PhysicObject(glm::vec3 position)
	: WorldUpVector(0.0f, 1.0f, 0.0f),	// TO BE MOVED TO A GLOBAL CONSTANT
	gravity(9.81f)					// TO BE MOVED TO A GLOBAL CONSTANT. Maybe use a gravity vector?
{

	// Position and movement
	Position = position;							// default : origin (0,0,0)
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);			// default : 0 units*s^(-1)
	Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);		// default : 0 units*s^(-2)
	Damping = 0.0f;									// default : 0.0f

	// Orientation vectors
	RightVector = glm::vec3(1.0f, 0.0f, 0.0f);		// default : x axis
	UpVector = glm::vec3(0.0f, 1.0f, 0.0f);			// default : y axis
	FrontVector = glm::vec3(0.0f, 0.0f, 1.0f);		// default : z axis

	// Mass
	Mass = 0.0f;									// default : 0 kg (immovable)
	kinematic = false;								// default : false

	// Collisions
	canCollide = true;								// default : true
	forcesApplied = glm::vec3(0.0f, 0.0f, 0.0f);	// default : 0 N
}

// Update physics state

void PhysicObject::UpdatePhysics(float deltaTime)
{
	if (kinematic) {
		// Kinematic object, only update position based on velocity
		Position += Velocity * deltaTime;
		return;
	}
	
	if (Mass <= 0.0f) {
		// Immovable object, do not update position or velocity
		forcesApplied = glm::vec3(0.0f);	// Reset applied forces
		return;
	}

	// Calculate local acceleration
	glm::vec3 localAcceleration = glm::vec3(0.0f); // Initialize local acceleration
	localAcceleration = forcesApplied / Mass;	// F = m*a  =>  a = F/m
	localAcceleration -= WorldUpVector * gravity;	// Add gravity
	localAcceleration += Acceleration;				// Add inner acceleration

	// Update velocity
	Velocity += localAcceleration * deltaTime;	// Integrate acceleration
	Velocity *= glm::exp(-Damping * deltaTime);	// Apply damping. Use exponential instead of linear to avoid instability.

	// Update position
	Position += Velocity * deltaTime;			// Integrate velocity

	// Reset applied forces
	forcesApplied = glm::vec3(0.0f);	// Since forces are instant, reset after each update
}