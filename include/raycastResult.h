#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

#include "physicObject.h"

class PhysicObject; // Forward declaration

class RaycastResult {
	public:	
		std::vector<PhysicObject*> hitObjects;	// List of the objects that were hit by the raycast. Empty vector if no object was hit.
		glm::vec3 Position;			// The world coordinates of the hit point.
		glm::vec3 Normal;			// The normal vector at the hit point.
		float Distance;				// The distance from the ray origin to the hit point.
};

enum RaycastMode {
	Closest,	// Return the closest hit object.
	Farthest,	// Return the farthest hit object.
	All			// Return all hit objects.
};

enum RaycastFilterMode {
	Include,	// Only consider objects in the InstanceList.
	Exclude		// Consider all objects except those in the InstanceList.
};

class RaycastParameters {
	public:
		glm::vec3 Origin;							// The origin point of the ray.
		glm::vec3 Direction;						// The direction vector of the ray (should be normalized).
		bool RespectCanCollide;						// Whether to respect the canCollide property of objects during the raycast.
		bool IgnoreKinematic;						// Whether to ignore kinematic objects during the raycast.
		std::vector<PhysicObject*> InstanceList;	// A list of objects to considere during the raycast.
		RaycastMode Mode;							// The raycast mode (Closest, Farthest, All).
		RaycastFilterMode FilterMode;				// The filter mode (Include, Exclude).
};