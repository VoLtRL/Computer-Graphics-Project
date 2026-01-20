#include "../include/physicObject.h"

#include "box.h"
#include "sphere.h"

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

float length2(const glm::vec3& v) {
	return glm::dot(v, v);
}

float ProjectOBB(const OBBCollision& box, const glm::vec3& axis) {
	return
		box.halfExtents.x * abs(glm::dot(axis, box.rotation[0])) +
		box.halfExtents.y * abs(glm::dot(axis, box.rotation[1])) +
		box.halfExtents.z * abs(glm::dot(axis, box.rotation[2]));
}


PhysicObject::PhysicObject(glm::vec3 position)
	: WorldUpVector(0.0f, 1.0f, 0.0f),	// TO BE MOVED TO A GLOBAL CONSTANT
	gravity(9.81f)					// TO BE MOVED TO A GLOBAL CONSTANT. Maybe use a gravity vector?
{

	// Position and movement
	Position = position;							// default : origin (0,0,0)
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);			// default : 0 units*s^(-1)
	Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);		// default : 0 units*s^(-2)
	Damping = 0.0f;									// default : 0.0f

	// Orientation matrix
	RotationMatrix = glm::mat4(1.0f);				// default : identity matrix

	// Mass
	Mass = 0.0f;									// default : 0 kg (immovable)
	kinematic = false;								// default : false

	// Collisions
	canCollide = true;								// default : true
	forcesApplied = glm::vec3(0.0f, 0.0f, 0.0f);	// default : 0 N
	Restitution = 0.5f;							// default : 0.5
	shapeType = BOX;								// default : BOX

	PhysicObject::allPhysicObjects.push_back(this); // Add this instance to the static list
}

void PhysicObject::ResolveCollision(
	PhysicObject* A,
	PhysicObject* B,
	const CollisionInfo& c
) {
	if (!c.hit) return;
	std::cout << "Collision detected with penetration: " << c.penetration << std::endl;

	// --- Correction de position ---
	float percent = 0.95f;
	float slop = 0.01f;

	glm::vec3 correction =
		std::max(c.penetration - slop, 0.0f)
		/ (A->InvMass + B->InvMass)
		* percent
		* c.normal;

	A->Position -= correction * A->InvMass;
	B->Position += correction * B->InvMass;

	// --- Vélocité relative ---
	glm::vec3 rv = B->Velocity - A->Velocity;
	float velAlongNormal = glm::dot(rv, c.normal);

	if (velAlongNormal > 0) return;

	float e = std::min(A->Restitution, B->Restitution);

	float j = -(1 + e) * velAlongNormal;
	j /= A->InvMass + B->InvMass;

	glm::vec3 impulse = j * c.normal;

	A->Velocity -= impulse * A->InvMass;
	B->Velocity += impulse * B->InvMass;
}


// Update physics state

void PhysicObject::UpdatePhysics(float deltaTime)
{

	if (Mass <= 0.0f) {
		// Immovable object, do not update position or velocity
		forcesApplied = glm::vec3(0.0f);	// Reset applied forces
		return;
	}

	if (kinematic) {
		// Kinematic object, only update position based on velocity
		Position += Velocity * deltaTime;

	}
	else {
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

	// Collisions
	for (PhysicObject* other : PhysicObject::allPhysicObjects) {
		if (other == this) continue;

		CollisionInfo info = checkCollision(this, other);
		ResolveCollision(this, other, info);
	}

}

CollisionInfo PhysicObject::Box2Box(PhysicObject* objA, PhysicObject* objB) {
	
	Box* a = dynamic_cast<Box*>(objA->collisionShape);
	Box* b = dynamic_cast<Box*>(objB->collisionShape);

	OBBCollision A;
	A.center = objA->Position;
	A.halfExtents = glm::vec3(a->w / 2.0f, a->h / 2.0f, a->d / 2.0f);
	A.rotation = glm::mat3(objA->RotationMatrix);

	OBBCollision B;
	B.center = objB->Position;
	B.halfExtents = glm::vec3(b->w / 2.0f, b->h / 2.0f, b->d / 2.0f);
	B.rotation = glm::mat3(objB->RotationMatrix);


	CollisionInfo result;

	glm::vec3 axes[15];
	int axisCount = 0;

	for (int i = 0; i < 3; i++) {
		axes[axisCount++] = A.rotation[i];
		axes[axisCount++] = B.rotation[i];
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			axes[axisCount++] = glm::cross(A.rotation[i], B.rotation[j]);
		}
	}

	glm::vec3 d = B.center - A.center;
	float minPenetration = FLT_MAX;
	glm::vec3 bestAxis;

	for (int i = 0; i < axisCount; i++) {
		glm::vec3 axis = axes[i];
		if (length2(axis) < 1e-6f) continue;

		axis = glm::normalize(axis);

		float dist = abs(glm::dot(d, axis));
		float rA = ProjectOBB(A, axis);
		float rB = ProjectOBB(B, axis);

		float overlap = rA + rB - dist;
		if (overlap < 0) {
			return result; // séparation
		}

		if (overlap < minPenetration) {
			minPenetration = overlap;
			bestAxis = axis;
		}
	}

	result.hit = true;
	result.normal = glm::dot(bestAxis, d) < 0 ? -bestAxis : bestAxis;
	result.penetration = minPenetration;

	return result;

}

CollisionInfo PhysicObject::Box2Sphere(PhysicObject* box, PhysicObject* sphere) {

	Box* boxShape = dynamic_cast<Box*>(box->collisionShape);
	Sphere* sphereShape = dynamic_cast<Sphere*>(sphere->collisionShape);

	SphereCollision s;
	s.center = sphere->Position;
	s.radius = sphereShape->radius;
	OBBCollision b;
	b.center = box->Position;
	b.halfExtents = glm::vec3(boxShape->w / 2.0f, boxShape->h / 2.0f, boxShape->d / 2.0f);
	b.rotation = glm::mat3(box->RotationMatrix);

	CollisionInfo result;

	glm::vec3 localCenter = glm::transpose(b.rotation) * (s.center - b.center);

	glm::vec3 closestPoint = glm::clamp(
		localCenter,
		-b.halfExtents,
		b.halfExtents
	);

	glm::vec3 worldClosest = b.center + b.rotation * closestPoint;
	glm::vec3 delta = s.center - worldClosest;

	float dist2 = glm::dot(delta, delta);
	if (dist2 > s.radius * s.radius) return result;

	float dist = sqrt(dist2);

	result.hit = true;
	result.normal = dist > 0.0f ? delta / dist : glm::vec3(0, 1, 0);
	result.penetration = s.radius - dist;

	return result;
}

CollisionInfo PhysicObject::Box2Capsule(PhysicObject* objA, PhysicObject* objB) {
	// Placeholder for Box to Capsule collision detection logic
	CollisionInfo result;

	return result; // No collision detected
}

CollisionInfo PhysicObject::Sphere2Sphere(PhysicObject* objA, PhysicObject* objB) {

	Sphere* a = dynamic_cast<Sphere*>(objA->collisionShape);
	Sphere* b = dynamic_cast<Sphere*>(objB->collisionShape);

	SphereCollision A;
	A.center = objA->Position;
	A.radius = a->radius;

	SphereCollision B;
	B.center = objB->Position;
	B.radius = b->radius;

	CollisionInfo result;

	glm::vec3 delta = B.center - A.center;
	float dist2 = glm::dot(delta, delta);
	float r = A.radius + B.radius;

	if (dist2 >= r * r) return result;

	float dist = sqrt(dist2);

	result.hit = true;
	result.normal = dist > 0.0f ? delta / dist : glm::vec3(0, 1, 0);
	result.penetration = r - dist;

	return result;
}

CollisionInfo PhysicObject::Sphere2Capsule(PhysicObject* objA, PhysicObject* objB) {
	// Placeholder for Sphere to Capsule collision detection logic
	CollisionInfo result;

	return result; // No collision detected
}

CollisionInfo PhysicObject::Capsule2Capsule(PhysicObject* objA, PhysicObject* objB) {
	// Placeholder for Capsule to Capsule collision detection logic
	CollisionInfo result;

	return result; // No collision detected
}

CollisionInfo PhysicObject::checkCollision(PhysicObject* objA, PhysicObject* objB) {
	if(!objA || !objB) {
		CollisionInfo result;
		return result; // No collision detected
	}

	ShapeType typeA = objA->shapeType;
	ShapeType typeB = objB->shapeType;
	if (!typeA || !typeB) {
		CollisionInfo result;
		return result; // No collision detected
	}
	// Simple collision detection based on Axis-Aligned Bounding Boxes (AABB)
	if (!objA->canCollide || !objB->canCollide) {
		CollisionInfo result;

		return result; // No collision detected
	}
	if (typeA == ShapeType::BOX) {
		// old was safely casted to NewType
		if (typeB == ShapeType::BOX) {
			return Box2Box(objA, objB);
		}
		else if (typeB == ShapeType::SPHERE) {
			return Box2Sphere(objA, objB);
		}
		/*
		else if (Capsule* w = dynamic_cast<Capsule*>(shapeB)) {
			return Box2Capsule(objA, objB);
		}
		*/
	}
	else if (typeA == ShapeType::SPHERE) {
		// old was safely casted to NewType
		if (typeB == ShapeType::BOX) {
			return Box2Sphere(objB, objA); // Reverse order
		}
		else if (typeB == ShapeType::SPHERE) {
			return Sphere2Sphere(objA, objB);
		}
		/*
		else if (Capsule* w = dynamic_cast<Capsule*>(objB)) {
			return Sphere2Capsule(objA, objB);
		}
		*/
	}
	/*
	else if (Capsule* v = dynamic_cast<Capsule*>(objA)) {
		// old was safely casted to NewType
		if (Box* w = dynamic_cast<Box*>(objB)) {
			return Box2Capsule(objB, objA); // Reverse order
		}
		else if (Sphere* w = dynamic_cast<Sphere*>(objB)) {
			return Sphere2Capsule(objB, objA); // Reverse order
		}
		else if (Capsule* w = dynamic_cast<Capsule*>(objB)) {
			return Capsule2Capsule(objA, objB);
		}
		
	}
	*/

	CollisionInfo result;

	return result; // No collision detected

}

std::string PhysicObject::ShapeTypeToString(ShapeType type) {
	switch (type) {
	case ShapeType::SPHERE:    return "SPHERE";
	case ShapeType::BOX:	   return "BOX";
	case ShapeType::CAPSULE:   return "CAPSULE";
	default:                   return "Unknown";
	}
}

std::ostream& operator<<(std::ostream& os, const PhysicObject& obj) {
	os << "PhysicObject(Position: [" << obj.Position.x << ", " << obj.Position.y << ", " << obj.Position.z
		<< "], Velocity: [" << obj.Velocity.x << ", " << obj.Velocity.y << ", " << obj.Velocity.z
		<< "], Acceleration: [" << obj.Acceleration.x << ", " << obj.Acceleration.y << ", " << obj.Acceleration.z
		<< "], Mass: " << obj.Mass
		<< ", Kinematic: " << obj.kinematic
		<< ", CanCollide: " << obj.canCollide
		<< ", ShapeType: " << PhysicObject::ShapeTypeToString(obj.shapeType)
		<< ")";
	return os;
}