#include "../include/physicObject.h"

#include "box.h"
#include "sphere.h"


float PhysicObject::Length2(const glm::vec3& v) {
	return glm::dot(v, v);
}

float ProjectOBB(const OBBCollision& box, const glm::vec3& axis) {
	return
		box.halfExtents.x * abs(glm::dot(axis, box.rotation[0])) +
		box.halfExtents.y * abs(glm::dot(axis, box.rotation[1])) +
		box.halfExtents.z * abs(glm::dot(axis, box.rotation[2]));
}



PhysicObject::PhysicObject(glm::vec3 position)
{

	// Position and movement
	Position = position;							// default : origin (0,0,0)
	Velocity = glm::vec3(0.0f, 0.0f, 0.0f);			// default : 0 units*s^(-1)
	Acceleration = glm::vec3(0.0f, 0.0f, 0.0f);		// default : 0 units*s^(-2)
	Damping = 0.0f;									// default : 0.0f
	Friction = 0.01f;								// default : 0.01f WARNING ultra sensitive, don't go too high (0.1 is enough for full stop)

	// Orientation matrix
	RotationMatrix = glm::mat4(1.0f);				// default : identity matrix

	// Mass
	Mass = 0.0f;									// default : 0 kg (immovable)
	InvMass = 0.0f;									// default : 0 kg^(-1)
	kinematic = false;								// default : false

	// Collisions
	canCollide = true;								// default : true
	forcesApplied = glm::vec3(0.0f, 0.0f, 0.0f);	// default : 0 N
	Restitution = 0.1f;							// default : 0.5
	shapeType = INVALID;								// default : BOX
	collisionShape = nullptr;					// default : nullptr

	PhysicObject::allPhysicObjects.push_back(this); // Add this instance to the static list
}

PhysicObject::~PhysicObject() {
    auto it = std::find(allPhysicObjects.begin(), allPhysicObjects.end(), this);
    if (it != allPhysicObjects.end()) {
        allPhysicObjects.erase(it);
    }
}
	

void PhysicObject::ResolveCollision(
	PhysicObject* A,
	PhysicObject* B,
	const CollisionInfo& c
) {
	if (!c.hit) return;
	//std::cout << "Collision detected with penetration: " << c.penetration << std::endl;

	A->BeforeCollide(B, c);
	B->BeforeCollide(A, c);

	float invMassSum = A->InvMass + B->InvMass;
	if (invMassSum == 0.0f) return;

	// --- S�curiser la normale ---
	glm::vec3 normal = c.normal;
	glm::vec3 AB = B->Position - A->Position;
	if (glm::dot(normal, AB) < 0.0f)
		normal = -normal;

	// --- Correction de position ---
	float percent = 0.8f;
	float slop = 0.01f;

	glm::vec3 correction =
		std::max(c.penetration - slop, 0.0f)
		/ invMassSum
		* percent
		* normal;

	A->Position -= correction * A->InvMass;
	B->Position += correction * B->InvMass;

	// --- V�locit� relative ---
	glm::vec3 rv = B->Velocity - A->Velocity;
	float velAlongNormal = glm::dot(rv, normal);

	if (velAlongNormal > 0.0f) return;

	float e = std::min(A->Restitution, B->Restitution);

	float j = -(1.0f + e) * velAlongNormal;
	j /= invMassSum;

	glm::vec3 impulse = j * normal;

	// IMPULSION CORRECTE
	A->Velocity -= impulse * A->InvMass;
	B->Velocity += impulse * B->InvMass;

	A->OnCollide(B, c);
	B->OnCollide(A, c);
}



// Update physics state

void PhysicObject::OnCollide(PhysicObject* other, CollisionInfo info) {
	// Placeholder for after-collision response logic
	return;
}

void PhysicObject::BeforeCollide(PhysicObject* other, CollisionInfo info) {
	// Placeholder for pre-collision logic
	return;
}

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
		// Reset applied forces
		forcesApplied = glm::vec3(0.0f);	// Since forces are instant, reset after each update
	}

}

CollisionInfo PhysicObject::Box2Box(PhysicObject* objA, PhysicObject* objB) {
	//std::cout << "Box-Box Collision Check" << std::endl;
	Box* a = static_cast<Box*>(objA->collisionShape);
	Box* b = static_cast<Box*>(objB->collisionShape);

	OBBCollision A;
	A.center = objA->Position;
	A.halfExtents = glm::vec3(a->w, a->h, a->d);
	A.rotation = glm::mat3(objA->RotationMatrix);

	OBBCollision B;
	B.center = objB->Position;
	B.halfExtents = glm::vec3(b->w, b->h, b->d);
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
		if (PhysicObject::Length2(axis) < 1e-6f) continue;

		axis = glm::normalize(axis);

		float dist = abs(glm::dot(d, axis));
		float rA = ProjectOBB(A, axis);
		float rB = ProjectOBB(B, axis);

		float overlap = rA + rB - dist;
		if (overlap < 0) {
			return result; // s�paration
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

CollisionInfo PhysicObject::Box2Sphere(PhysicObject* boxObj, PhysicObject* sphereObj) {
	//std::cout << "Box-Sphere Collision Check" << std::endl;
	Box* boxShape = static_cast<Box*>(boxObj->collisionShape);
	Sphere* sphereShape = static_cast<Sphere*>(sphereObj->collisionShape);

	OBBCollision box;
	box.center = boxObj->Position;
	box.halfExtents = glm::vec3(boxShape->w, boxShape->h, boxShape->d);
	box.rotation = glm::mat3(boxObj->RotationMatrix);

	SphereCollision sphere;
	sphere.center = sphereObj->Position;
	sphere.radius = sphereShape->radius;

	CollisionInfo result;

	// 1. Matrice inverse de rotation
	glm::mat4 invRot = glm::inverse(box.rotation);

	// 2. Centre de la sph�re en espace local de la bo�te
	glm::vec3 localCenter =
		glm::vec3(invRot * glm::vec4(sphere.center - box.center, 1.0f));

	// 3. Point le plus proche sur l'AABB locale
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(localCenter.x, -box.halfExtents.x, box.halfExtents.x);
	closestPoint.y = glm::clamp(localCenter.y, -box.halfExtents.y, box.halfExtents.y);
	closestPoint.z = glm::clamp(localCenter.z, -box.halfExtents.z, box.halfExtents.z);

	// 4. Vecteur entre la sph�re et la bo�te
	glm::vec3 delta = localCenter - closestPoint;
	float distanceSq = glm::dot(delta, delta);

	if (distanceSq > sphere.radius * sphere.radius){
		result.hit = false;
		result.penetration = 0.0f;
		return result;
	}

	//std::cout << "distanceSq = " << distanceSq << std::endl;
	//std::cout << "radiusSq   = " << sphere.radius * sphere.radius << std::endl;

	result.hit = true;
	float distance = sqrt(distanceSq);
	result.penetration = sphere.radius - distance;

	// 5. Handle case when sphere center is inside the box
	if (distance < 0.0001f) {
		// Trouver la face la plus proche
		glm::vec3 absLocal = glm::abs(localCenter);
		glm::vec3 d = box.halfExtents - absLocal;

		if (d.x < d.y && d.x < d.z)
			delta = glm::vec3(glm::sign(localCenter.x), 0, 0);
		else if (d.y < d.z)
			delta = glm::vec3(0, glm::sign(localCenter.y), 0);
		else
			delta = glm::vec3(0, 0, glm::sign(localCenter.z));

		distance = 0.0f;
	}

	// 6. Normale en espace local
	glm::vec3 localNormal = glm::normalize(delta);

	// 7. Repasser la normale en espace monde
	result.normal =
		glm::normalize(glm::vec3(box.rotation * glm::vec4(localNormal, 0.0f)));

	// 8. P�n�tration
	result.penetration = sphere.radius - distance;

	return result;
}

CollisionInfo PhysicObject::Box2Capsule(PhysicObject* objA, PhysicObject* objB) {
	// Placeholder for Box to Capsule collision detection logic
	CollisionInfo result;

	return result; // No collision detected
}

CollisionInfo PhysicObject::Sphere2Sphere(PhysicObject* objA, PhysicObject* objB) {

	Sphere* a = static_cast<Sphere*>(objA->collisionShape);
	Sphere* b = static_cast<Sphere*>(objB->collisionShape);

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
	if (!objA->canCollide || !objB->canCollide) {
		std::cout << "One of the PhysicObjects cannot collide." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	std::cout << "Checking collision between " << objA->name << " and " << objB->name << std::endl;

	if(!objA || !objB) {
		std::cout << "One of the PhysicObjects is null." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	ShapeType typeA = objA->shapeType;
	ShapeType typeB = objB->shapeType;
	if (typeA == INVALID || typeB == INVALID) {
		std::cout << "One of the PhysicObjects has an invalid ShapeType." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	Shape* shapeA = objA->collisionShape;
	Shape* shapeB = objB->collisionShape;
	if (!shapeA || !shapeB) {
		std::cout << "One of the PhysicObjects has a null collision shape." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}


	if (typeA == BOX) {
		// old was safely casted to NewType
		if (typeB == BOX) {
			return Box2Box(objA, objB);
		}
		else if (typeB == SPHERE) {
			return Box2Sphere(objA, objB);
		}
		else if (typeB == CAPSULE) {
			return Box2Capsule(objA, objB);
		}
	}
	else if (typeA == SPHERE) {
		// old was safely casted to NewType
		if (typeB == BOX) {
			return Box2Sphere(objB, objA); // Reverse order
		}
		else if (typeB == SPHERE) {
			return Sphere2Sphere(objA, objB);
		}
		else if (typeB == CAPSULE) {
			return Sphere2Capsule(objA, objB);
		}
	}
	else if (typeA == CAPSULE) {
		// old was safely casted to NewType
		if (typeB == BOX) {
			return Box2Capsule(objB, objA); // Reverse order
		}
		else if (typeB == SPHERE) {
			return Sphere2Capsule(objB, objA); // Reverse order
		}
		else if (typeB == CAPSULE) {
			return Capsule2Capsule(objA, objB);
		}
		
	}

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
	os << "PhysicObject(Name : " << obj.name
		<< "   \n\tPosition: [" << obj.Position.x << ", " << obj.Position.y << ", " << obj.Position.z
		<< "], \n\tVelocity: [" << obj.Velocity.x << ", " << obj.Velocity.y << ", " << obj.Velocity.z
		<< "], \n\tAcceleration: [" << obj.Acceleration.x << ", " << obj.Acceleration.y << ", " << obj.Acceleration.z
		<< "], \n\tMass: " << obj.Mass
		<< ", \n\tKinematic: " << obj.kinematic
		<< ", \n\tCanCollide: " << obj.canCollide
		<< ", \n\tShapeType: " << PhysicObject::ShapeTypeToString(obj.shapeType)
		<< ")";
	return os;
}