#pragma once

#include "physicShapeObject.h"

class Pickup : public PhysicShapeObject {
	public:
		Pickup(Shape* = nullptr, glm::vec3 Position = glm::vec3(0.0f));

		float lifetime = 0.0f; // seconds

		void BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
};