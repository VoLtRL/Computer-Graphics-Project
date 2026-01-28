#pragma once

#include "physicShapeObject.h"

class Pickup : public PhysicShapeObject {
	public:
		Pickup(Shape* = nullptr, glm::vec3 Position = glm::vec3(0.0f));
		~Pickup() {};

		float lifetime = -1.0f; // seconds
		bool toBeDeleted = false;

		void BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
		void AfterCollide(CollisionInfo info, float deltaTime) override;
};