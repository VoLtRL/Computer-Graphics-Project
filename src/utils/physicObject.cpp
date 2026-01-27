#include "../include/physicObject.h"

#include "box.h"
#include "sphere.h"
#include "capsule.h"


float PhysicObject::Length2(const glm::vec3& v) {
	return glm::dot(v, v);
}

float PhysicObject::ProjectOBB(const OBBCollision& box, const glm::vec3& axis) {
	return
		box.halfExtents.x * glm::abs(glm::dot(axis, box.rotation[0])) +
		box.halfExtents.y * glm::abs(glm::dot(axis, box.rotation[1])) +
		box.halfExtents.z * glm::abs(glm::dot(axis, box.rotation[2]));
}

glm::vec3 PhysicObject::ClosestPointAABB(
	const glm::vec3& p,
	const glm::vec3& min,
	const glm::vec3& max
) {
	return glm::clamp(p, min, max);
}

void ClosestPointsSegmentSegment(
	const glm::vec3& p1, const glm::vec3& q1,
	const glm::vec3& p2, const glm::vec3& q2,
	glm::vec3& c1, glm::vec3& c2
) {
	glm::vec3 d1 = q1 - p1;
	glm::vec3 d2 = q2 - p2;
	glm::vec3 r = p1 - p2;

	float a = glm::dot(d1, d1);
	float e = glm::dot(d2, d2);
	float f = glm::dot(d2, r);

	float s, t;

	if (a <= 1e-6f && e <= 1e-6f) {
		c1 = p1;
		c2 = p2;
		return;
	}

	if (a <= 1e-6f) {
		s = 0.0f;
		t = glm::clamp(f / e, 0.0f, 1.0f);
	}
	else {
		float c = glm::dot(d1, r);

		if (e <= 1e-6f) {
			t = 0.0f;
			s = glm::clamp(-c / a, 0.0f, 1.0f);
		}
		else {
			float b = glm::dot(d1, d2);
			float denom = a * e - b * b;

			if (denom != 0.0f)
				s = glm::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			else
				s = 0.0f;

			t = (b * s + f) / e;

			if (t < 0.0f) {
				t = 0.0f;
				s = glm::clamp(-c / a, 0.0f, 1.0f);
			}
			else if (t > 1.0f) {
				t = 1.0f;
				s = glm::clamp((b - c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;
}

glm::vec3 PhysicObject::ClosestPointSegmentAABB(
	const glm::vec3& A,
	const glm::vec3& B,
	const glm::vec3& boxMin,
	const glm::vec3& boxMax
) {
	float t = 0.0f;
	glm::vec3 d = B - A;

	glm::vec3 p = A;

	for (int i = 0; i < 3; ++i) {
		if (std::abs(d[i]) < 1e-6f) {
			p[i] = glm::clamp(A[i], boxMin[i], boxMax[i]);
		}
		else {
			float ood = 1.0f / d[i];
			float t1 = (boxMin[i] - A[i]) * ood;
			float t2 = (boxMax[i] - A[i]) * ood;

			if (t1 > t2) std::swap(t1, t2);

			t = glm::max(t, t1);
			t = glm::min(t, t2);
		}
	}

	t = glm::clamp(t, 0.0f, 1.0f);
	return A + d * t;
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
	forcesApplied = glm::vec3(0.0f, 0.0f, 0.0f);	// default : 0 N
	Restitution = 0.1f;							// default : 0.5
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
	const CollisionInfo& c,
	float deltaTime
) {
	if (!c.hit) return;
	//std::cout << "Collision detected with penetration: " << c.penetration << std::endl;

	CollisionResponse repA = A->collisionResponse;
	CollisionResponse repB = B->collisionResponse;

	bool doPhysical = (repA == CollisionResponse::CR_PHYSICAL || repA == CollisionResponse::CR_BOTH)
		&& (repB == CollisionResponse::CR_PHYSICAL || repB == CollisionResponse::CR_BOTH);

	bool doTrigger = (repA == CollisionResponse::CR_TRIGGER || repA == CollisionResponse::CR_BOTH)
		&& (repB == CollisionResponse::CR_TRIGGER || repB == CollisionResponse::CR_BOTH);

	if (!doPhysical && !doTrigger) return;
	if (doTrigger){
		A->BeforeCollide(B, c, deltaTime);
		B->BeforeCollide(A, c, deltaTime);
	}

	if (doPhysical){
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

		// --- FRICTION ---
		glm::vec3 tangent =
			rv - glm::dot(rv, normal) * normal;

		if (PhysicObject::Length2(tangent) > 1e-6f) {
			tangent = glm::normalize(tangent);

			float mu = std::sqrt(A->Friction * B->Friction);

			float jt = -glm::dot(rv, tangent);
			jt /= (A->InvMass + B->InvMass);

			float maxFriction = j * mu;
			jt = glm::clamp(jt, -maxFriction, maxFriction);

			glm::vec3 frictionImpulse = jt * tangent;

			A->Velocity -= frictionImpulse * A->InvMass;
			B->Velocity += frictionImpulse * B->InvMass;
		}
	}

	if (doTrigger)
	{
	A->OnCollide(B, c, deltaTime);
	B->OnCollide(A, c, deltaTime);
	}

}



// Update physics state

void PhysicObject::OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime) {
	// Placeholder for after-collision response logic
	return;
}

void PhysicObject::BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) {
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

		float dist = glm::abs(glm::dot(d, axis));
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

	// 8. Pénétration
	result.penetration = sphere.radius - distance;

	return result;
}

CollisionInfo PhysicObject::Box2Capsule(PhysicObject* objA, PhysicObject* objB) {
	CollisionInfo result;

	Box* boxShape = static_cast<Box*>(objA->collisionShape);
	Capsule* capShape = static_cast<Capsule*>(objB->collisionShape);

	OBBCollision box;
	box.center = objA->Position;
	box.halfExtents = glm::vec3(boxShape->w, boxShape->h, boxShape->d);
	box.rotation = glm::mat3(objA->RotationMatrix);

	CapsuleCollision cap;
	cap.A = objB->Position + objB->GetUpVector() * (capShape->height / 2.0f);
	cap.B = objB->Position - objB->GetUpVector() * (capShape->height / 2.0f);
	cap.radius = capShape->radius;


	// 1️⃣ Rotation pure (plus stable que inverse(mat4))
	glm::mat3 rot = glm::mat3(box.rotation);
	glm::mat3 invRot = glm::transpose(rot);

	// 2️⃣ Capsule en espace local de la boîte
	glm::vec3 localA = invRot * (cap.A - box.center);
	glm::vec3 localB = invRot * (cap.B - box.center);

	glm::vec3 boxMin = -box.halfExtents;
	glm::vec3 boxMax = box.halfExtents;

	// 3️⃣ Point du segment le plus proche de la boîte
	glm::vec3 closest = ClosestPointSegmentAABB(
		localA, localB, boxMin, boxMax
	);

	// 4️⃣ Point le plus proche SUR la boîte
	glm::vec3 boxPoint = ClosestPointAABB(
		closest, boxMin, boxMax
	);

	// 5️⃣ Distance capsule ↔ boîte
	glm::vec3 delta = closest - boxPoint;
	float distSq = glm::dot(delta, delta);

	if (distSq > cap.radius * cap.radius)
		return result; // pas collision 🌱

	// 6️⃣ Collision confirmée
	float distance = std::sqrt(distSq);

	result.hit = true;
	result.penetration = cap.radius - distance;

	// Normale locale
	glm::vec3 localNormal;
	if (distance < 1e-6f) {
		// Capsule au cœur de la boîte → on pousse vers la sortie
		glm::vec3 d = glm::abs(closest) - box.halfExtents;

		if (d.x > d.y && d.x > d.z)
			localNormal = glm::vec3(glm::sign(closest.x), 0, 0);
		else if (d.y > d.z)
			localNormal = glm::vec3(0, glm::sign(closest.y), 0);
		else
			localNormal = glm::vec3(0, 0, glm::sign(closest.z));
	}
	else {
		localNormal = glm::normalize(delta);
	}

	// 7️⃣ Normale en espace monde
	result.normal = glm::normalize(rot * localNormal);

	return result;
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
	CollisionInfo result;

	Sphere* sphShape = static_cast<Sphere*>(objA->collisionShape);
	Capsule* capShape = static_cast<Capsule*>(objB->collisionShape);

	SphereCollision sph;
	sph.center = objA->Position;
	sph.radius = sphShape->radius;

	CapsuleCollision cap;
	cap.A = objB->Position + objB->GetUpVector() * (capShape->height / 2.0f);
	cap.B = objB->Position - objB->GetUpVector() * (capShape->height / 2.0f);
	cap.radius = capShape->radius;


	// 1️⃣ Projection du centre de la sphère sur le segment
	glm::vec3 AB = cap.B - cap.A;
	float abLenSq = glm::dot(AB, AB);

	float t = 0.0f;
	if (abLenSq > 1e-6f) {
		t = glm::dot(sph.center - cap.A, AB) / abLenSq;
		t = glm::clamp(t, 0.0f, 1.0f);
	}

	glm::vec3 closest = cap.A + t * AB;

	// 2️⃣ Distance entre la sphère et la capsule
	glm::vec3 delta = sph.center - closest;
	float distSq = glm::dot(delta, delta);

	float radiusSum = cap.radius + sph.radius;

	if (distSq > radiusSum * radiusSum)
		return result; // pas collision 

	// 3️⃣ Collision confirmée
	float distance = std::sqrt(distSq);

	result.hit = true;
	result.penetration = radiusSum - distance;

	// 4️⃣ Normale
	if (distance < 1e-6f) {
		// centre pile sur l'axe → normale arbitraire
		glm::vec3 axis = cap.B - cap.A;
		result.normal = glm::normalize(axis);
	}
	else {
		result.normal = glm::normalize(delta);
	}

	return result;
}

CollisionInfo PhysicObject::Capsule2Capsule(PhysicObject* objA, PhysicObject* objB) {
	CollisionInfo result;

	glm::vec3 cA, cB;

	Capsule* capShapeA = static_cast<Capsule*>(objA->collisionShape);
	Capsule* capShapeB = static_cast<Capsule*>(objB->collisionShape);

	glm::vec3 capA_start = objA->Position + objA->GetUpVector() * (capShapeA->height / 2.0f);
	glm::vec3 capA_end = objA->Position - objA->GetUpVector() * (capShapeA->height / 2.0f);

	glm::vec3 capB_start = objB->Position + objB->GetUpVector() * (capShapeB->height / 2.0f);
	glm::vec3 capB_end = objB->Position - objB->GetUpVector() * (capShapeB->height / 2.0f);


	// 1️⃣ Points les plus proches entre les deux segments
	ClosestPointsSegmentSegment(
		capA_start, capA_end,
		capB_start, capB_end,
		cA, cB
	);

	// 2️⃣ Distance entre ces points
	glm::vec3 delta = cB - cA;
	float distSq = glm::dot(delta, delta);

	float radiusSum = capShapeA->radius + capShapeB->radius;

	if (distSq > radiusSum * radiusSum)
		return result; // pas de collision 🌿

	// 3️⃣ Collision confirmée
	float distance = std::sqrt(distSq);

	result.hit = true;

	// Cas dégénéré : segments parfaitement alignés
	if (distance < 1e-6f) {
		glm::vec3 axis = capA_end - capA_start;
		result.normal = glm::normalize(axis);
		result.penetration = radiusSum;
	}
	else {
		result.normal = glm::normalize(delta);
		result.penetration = radiusSum - distance;
	}

	return result;
}

CollisionInfo PhysicObject::checkCollision(PhysicObject* objA, PhysicObject* objB) {

	if (!objA || !objB) {
		std::cout << "One of the PhysicObjects is null." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}


	if (!((objA->collisionMask & objB->collisionGroup) && (objB->collisionMask & objA->collisionGroup))) {
		std::cout << "These objects can't collide with each others." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	CollisionResponse repA = objA->collisionResponse;
	CollisionResponse repB = objB->collisionResponse;

	bool doPhysical = (repA == CollisionResponse::CR_PHYSICAL || repA == CollisionResponse::CR_BOTH) 
		&& (repB == CollisionResponse::CR_PHYSICAL || repB == CollisionResponse::CR_BOTH);

	bool doTrigger = (repA == CollisionResponse::CR_TRIGGER || repA == CollisionResponse::CR_BOTH) 
		&& (repB == CollisionResponse::CR_TRIGGER || repB == CollisionResponse::CR_BOTH);

	if (!doPhysical && !doTrigger) {
		std::cout << "One of the PhysicObjects cannot collide and cannot touch." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	std::cout << "Checking collision between " << objA->name << " and " << objB->name << std::endl;

	Shape* shapeA = objA->collisionShape;
	Shape* shapeB = objB->collisionShape;
	if (!shapeA || !shapeB) {
		std::cout << "One of the PhysicObjects has a null collision shape." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	ShapeType typeA = shapeA->shapeType;
	ShapeType typeB = shapeB->shapeType;
	if (typeA == ShapeType::ST_INVALID || typeB == ShapeType::ST_INVALID) {
		std::cout << "One of the PhysicObjects has an invalid ShapeType." << std::endl;
		CollisionInfo result;
		return result; // No collision detected
	}

	if (typeA == ShapeType::ST_BOX) {
		// old was safely casted to NewType
		if (typeB == ShapeType::ST_BOX) {
			return Box2Box(objA, objB);
		}
		else if (typeB == ShapeType::ST_SPHERE) {
			return Box2Sphere(objA, objB);
		}
		else if (typeB == ShapeType::ST_CAPSULE) {
			return Box2Capsule(objA, objB);
		}
	}
	else if (typeA == ShapeType::ST_SPHERE) {
		// old was safely casted to NewType
		if (typeB == ShapeType::ST_BOX) {
			return Box2Sphere(objB, objA); // Reverse order
		}
		else if (typeB == ShapeType::ST_SPHERE) {
			return Sphere2Sphere(objA, objB);
		}
		else if (typeB == ShapeType::ST_CAPSULE) {
			return Sphere2Capsule(objA, objB);
		}
	}
	else if (typeA == ShapeType::ST_CAPSULE) {
		// old was safely casted to NewType
		if (typeB == ShapeType::ST_BOX) {
			return Box2Capsule(objB, objA); // Reverse order
		}
		else if (typeB == ShapeType::ST_SPHERE) {
			return Sphere2Capsule(objB, objA); // Reverse order
		}
		else if (typeB == ShapeType::ST_CAPSULE) {
			return Capsule2Capsule(objA, objB);
		}
		
	}

	CollisionInfo result;

	return result; // No collision detected

}

std::ostream& operator<<(std::ostream& os, const PhysicObject& obj) {
	os << "PhysicObject(Name : " << obj.name
		<< "   \n\tPosition: [" << obj.Position.x << ", " << obj.Position.y << ", " << obj.Position.z
		<< "], \n\tVelocity: [" << obj.Velocity.x << ", " << obj.Velocity.y << ", " << obj.Velocity.z
		<< "], \n\tAcceleration: [" << obj.Acceleration.x << ", " << obj.Acceleration.y << ", " << obj.Acceleration.z
		<< "], \n\tMass: " << obj.Mass
		<< ", \n\tKinematic: " << obj.kinematic
		<< ", \n\tCollisionResponde: " << obj.collisionResponse
		<< ", \n\tShapeType: " << obj.collisionShape->shapeType
		<< ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const CollisionResponse& cr) {
	switch (cr) {
	case CollisionResponse::CR_NONE:
		os << "NONE";
		break;
	case CollisionResponse::CR_TRIGGER:
		os << "TRIGGER";
		break;
	case CollisionResponse::CR_PHYSICAL:
		os << "PHYSICAL";
		break;
	case CollisionResponse::CR_BOTH:
		os << "BOTH";
		break;
	default:
		os << "Unknown CollisionResponse";
		break;
	}
	return os;
}


std::ostream& operator<<(std::ostream& os, const ShapeType& st) {
	switch (st) {
	case ShapeType::ST_BOX:
		os << "BOX";
		break;
	case ShapeType::ST_SPHERE:
		os << "SPHERE";
		break;
	case ShapeType::ST_CAPSULE:
		os << "CAPSULE";
		break;
	case ShapeType::ST_INVALID:
		os << "INVALID";
		break;
	default:
		os << "Unknown ShapeType";
		break;
	}
	return os;
}
