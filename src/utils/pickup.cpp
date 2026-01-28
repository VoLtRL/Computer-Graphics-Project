#include "pickup.h"
#include "player.h"

Pickup::Pickup(Shape* shape, glm::vec3 position)
	: PhysicShapeObject(shape, position) {
	this->collisionGroup = CG_PICKUP;
	this->collisionMask = CG_PRESETS_PICKUP;
	this->SetMass(0.0f);
	this->kinematic=true;
	this->name = "PLACEHOLDER";
	this->collisionResponse = CollisionResponse::CR_TRIGGER;
}

void Pickup::BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{
	if (toBeDeleted || !info.hit) return;
	Player* player = static_cast<Player*>(other);
	if (player) {
		if (name == "HealthPack") {
			player->heal(25.0f);
		}
		else {
			player->AddPickup(this, lifetime);
		}
		toBeDeleted = true;
	}
}

void Pickup::AfterCollide(CollisionInfo info, float deltaTime)
{
	if (toBeDeleted){
		markForDeletion();
		std::cout << "Marked" << std::endl;
	}
}