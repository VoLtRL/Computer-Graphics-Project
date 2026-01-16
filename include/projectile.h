#pragma once

#include "physicShapeObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Projectile : public PhysicShapeObject {

public:
	Projectile(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f), float speed = 20.0f, float damage = 10.0f, float range = 50.0f);

	void update(float deltaTime);

	bool isActive() const { return active; }


private:

	bool active;
	// effects on hit, damage, speed, size, etc.
	float projectileSpeed;
	float damage;
	float size;
	float range;
	float traveledDistance;

};