#include "pickup.h"
#include "sphere.h"

Pickup::Pickup(Shape* shape, glm::vec3 Position, float radius)
	: PhysicShapeObject(shape, Position) {
	// Pickup specific initialization if needed
	this->collisionGroup = CG_PICKUP;
	this->collisionMask = CG_PRESETS_PICKUP;

	Shader* StandardShader = ResourceManager::GetShader("standard");
	Sphere* sphere = new Sphere(StandardShader, radius);
	this->collisionShape = sphere;
	this->collisionResponse = CollisionResponse::CR_TRIGGER;
	this->Mass = 1.0f;
	this->Restitution = 0.0f;
	this->Damping = 0.5f;
	this->Friction = 0.5f;
	this->name = "PickupName";4
	this->lifetime = 0.0f;
}

void Pickup::BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) {
	Player* p = static_cast<Player*>(other);
	if (p && p!=nullptr) {
		p->AddPickup(name);
	}
}

void ~Pickup::Pickup() {
	
}