#pragma once

#include <../external/glm/glm/glm.hpp>
#include <../external/glm/glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <string>

class physicShapeObject; // Forward declaration


struct CollisionInfo {
	bool hit = false;
	glm::vec3 normal = glm::vec3(0.0f);
	float penetration = 0.0f;
};

struct OBBCollision {
	glm::vec3 center;
	glm::vec3 halfExtents; // (w/2, h/2, d/2)
	glm::mat3 rotation;    // orientation
};

struct SphereCollision {
	glm::vec3 center;
	float radius;
};

class Shape;

enum ShapeType {
	BOX,
	SPHERE,
	CAPSULE,
	INVALID
};

class PhysicObject {

public:
	PhysicObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)
	);

	std::string name = "";

	// Position and movement
	glm::vec3 Position;			// The object's world position.
	glm::vec3 Velocity;			// The object's velocity, updates position.
	glm::vec3 Acceleration;		// The object's acceleration, updates velocity.
	float Damping;				// Damping strength, updates velocity.
	float Friction;				// Friction coefficient, updates velocity.

	// Orientation vectors
	glm::mat4 RotationMatrix;		// The object's rotation matrix.

	inline static const glm::vec3 WorldUpVector = glm::vec3(0.0f,1.0f,0.0f);	// The world's upward vector.

	// Mass
	float Mass;					// The object's mass in kg.
	float InvMass;				// The object's inverse mass in kg^(-1).
	bool kinematic;				// True if the object is kinematic (not affected by forces).
	inline static const float gravity = 9.8f;		// Gravitational acceleration in m*s^(-2). TO BE MOVED TO A GLOBAL CONSTANT

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

	void ApplyForce(const glm::vec3& force) {
		forcesApplied += force;
	}

	glm::vec3 GetFrontVector() {
		return glm::vec3(RotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	}

	glm::vec3 GetRightVector() {
		return glm::vec3(RotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	}

	glm::vec3 GetUpVector() {
		return glm::vec3(RotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	}


	inline static std::vector<PhysicObject*> allPhysicObjects{}; // Static list of all PhysicObject instances

	virtual void BeforeCollide(PhysicObject* other, CollisionInfo info);
	virtual void OnCollide(PhysicObject* other, CollisionInfo info);

	static float Length2(const glm::vec3& v); // Static method to compute squared length of a vector

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