#include "pickup.h"
#include "player.h"

Pickup::Pickup(Shape* shape, glm::vec3 position)
	: PhysicShapeObject(shape, position) {
	this->collisionGroup = CG_PICKUP;
	this->collisionMask = CG_PRESETS_PICKUP;
	this->SetMass(1.0f);
	this->Friction = 0.5f;
	this->Damping = 0.9f;
	this->name = "PLACEHOLDER";
	this->collisionResponse = CollisionResponse::CR_BOTH;
}

void Pickup::BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{
	if (!info.hit) return;
	Player* player = static_cast<Player*>(other);
	if (player) {
		player->AddPickup(this, lifetime);
		PhysicObject::deleteObject(this);
		delete this;
	}
}