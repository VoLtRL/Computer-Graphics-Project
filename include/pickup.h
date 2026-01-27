#pragma once

#include "physicShapeObject.h"

class Pickup : public PhysicShapeObject {
	public:
		Pickup(Shape* = nullptr, glm::vec3 Position = glm::vec3(0.0f), float radius = 1.0f);

		void BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
		void OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
		void ~Pickup() override;

		std::string name;
		float lifetime;
};