#pragma once

#include "physicShapeObject.h"

class Pickup : public PhysicShapeObject {
	public:
		Pickup(Shape* = nullptr, glm::vec3 Position = glm::vec3(0.0f));

		void OnCollide()
};