#include "projectile.h"
#include "sphere.h"

Projectile::Projectile(Shape* shape, glm::vec3 position, float speed, float damage, float range)
    : PhysicShapeObject(shape, position),    
      projectileSpeed(speed),
      damage(damage),
      range(range),
      traveledDistance(0.0f),
      active(true)
      {
    // Set initial velocity in the forward direction
    Velocity = GetFrontVector() * projectileSpeed;
    Mass = 1.0f; // Set a default mass
    kinematic = false; // Projectiles are affected by physics
    }

void Projectile::update(float deltaTime)
{
    if (!active){
        return;
    }

    // update traveled distance
    traveledDistance += glm::length(Velocity * deltaTime);

    // deactivate if exceeded range
    if (traveledDistance >= range || (glm::abs(Velocity.y)<0.01f && glm::abs(Velocity.x)<0.01f && glm::abs(Velocity.z)<0.01f)) {
        active = false;
    }

    /* std::cout << "Projectile position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl; */
}

