#include "projectile.h"
#include "sphere.h"

Projectile::Projectile(Shape* shape, glm::vec3 position, float speed, float damage, float range)
    : PhysicShapeObject(shape, position),    
      projectileSpeed(speed),
      damage(damage),
      range(range),
      traveledDistance(0.0f),
      active(true),
      Mass(1.0f),
      Velocity(glm::normalize(FrontVector) * projectileSpeed),
      canCollide(true),
      kinematic(false),
      gravity(9.81f),
      Damping(0.0f),
        
{
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
