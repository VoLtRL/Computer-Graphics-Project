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
    Velocity = FrontVector * projectileSpeed;
    Mass = 1.0f; // Set a default mass
    kinematic = false; // Projectiles are affected by physics
    

      }

void Projectile::update(float deltaTime)
{
    if (!active) return;

    // update physics
    UpdatePhysics(deltaTime);

    // update traveled distance
    traveledDistance += glm::length(Velocity * deltaTime);

    // deactivate if exceeded range
    if (traveledDistance >= range) {
        active = false;
    }

    std::cout << "Projectile position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl;
}
