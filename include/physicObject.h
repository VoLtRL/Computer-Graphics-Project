#pragma once

#include <../external/glm/glm/glm.hpp>
#include <../external/glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

class physicShapeObject; // Forward declaration

struct CollisionInfo;
struct OBBCollision;
struct SphereCollision;

class Shape;

enum ShapeType {
	BOX,
	SPHERE,
	CAPSULE
};

class PhysicObject {

public:
	PhysicObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)
	);

	// Position and movement
	glm::vec3 Position;			// The object's world position.
	glm::vec3 Velocity;			// The object's velocity, updates position.
	glm::vec3 Acceleration;		// The object's acceleration, updates velocity.
	float Damping;				// Damping strength, updates velocity.

	// Orientation vectors
	glm::mat4 RotationMatrix;		// The object's rotation matrix.

	// A CHANGER
	glm::vec3 FrontVector;		// The object's forward vector. TO BE MOVED TO A GLOBAL CONSTANT
	glm::vec3 RightVector;		// The object's right vector. TO BE MOVED TO A GLOBAL CONSTANT
	glm::vec3 UpVector;			// The object's upward vector. TO BE MOVED TO A GLOBAL CONSTANT
	//A CHANGER


	const glm::vec3 WorldUpVector;	// The world's upward vector. TO BE MOVED TO A GLOBAL CONSTANT

	// Mass
	float Mass;					// The object's mass in kg.
	float InvMass;				// The object's inverse mass in kg^(-1).
	bool kinematic;				// True if the object is kinematic (not affected by forces).
	const float gravity;		// Gravitational acceleration in m*s^(-2). TO BE MOVED TO A GLOBAL CONSTANT

	// Collisions
	bool canCollide;			// True if the object can collide with other collidable objects.
	glm::vec3 forcesApplied;	// Instant forces in Newton, updates acceleration. Does not include damping, gravity or inner accelerations.
	Shape* collisionShape;		// The object's collision shape.
	float Restitution;			// Coefficient of restitution (bounciness) [0, 1]
	ShapeType shapeType;		// Type of the collision shape

	// Update physics state
	void UpdatePhysics(float deltaTime);

	void SetMass(float mass) {
		Mass = mass;
		InvMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
	}

	inline static std::vector<PhysicObject*> allPhysicObjects{}; // Static list of all PhysicObject instances

	static CollisionInfo Box2Box(PhysicObject* objA, PhysicObject* objB); // Static method for AABB collision detection
	static CollisionInfo Box2Sphere(PhysicObject* objA, PhysicObject* objB); // Static method for Box to Sphere collision detection
	static CollisionInfo Box2Capsule(PhysicObject* objA, PhysicObject* objB); // Static method for Box to Capsule collision detection
	static CollisionInfo Sphere2Sphere(PhysicObject* objA, PhysicObject* objB); // Static method for Sphere to Sphere collision detection
	static CollisionInfo Sphere2Capsule(PhysicObject* objA, PhysicObject* objB); // Static method for Sphere to Capsule collision detection
	static CollisionInfo Capsule2Capsule(PhysicObject* objA, PhysicObject* objB); // Static method for Capsule to Capsule collision detection
	static CollisionInfo checkCollision(PhysicObject* objA, PhysicObject* objB); // Static method to check collision between two PhysicObjects

	static void ResolveCollision(PhysicObject* objA, PhysicObject* objB, const CollisionInfo& collisionInfo);
	static std::string ShapeTypeToString(ShapeType type);

};
std::ostream& operator<<(std::ostream& os, const PhysicObject& obj);