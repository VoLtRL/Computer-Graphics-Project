#include "projectile.h"
#include "sphere.h"
#include "enemy.h"

Projectile::Projectile(Shape* shape, glm::vec3 position, float speed, float damage, float range)
    : PhysicShapeObject(shape, position),    
      projectileSpeed(speed),
      damage(damage),
      range(range),
      traveledDistance(0.0f),
      active(true),
      pierce(0)
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
    if (traveledDistance >= range || (glm::abs(Velocity.y)<=0.05f && glm::abs(Velocity.x)<=0.05f && glm::abs(Velocity.z)<=0.05f)) {
        active = false;
    }
    /*
    std::cout << "Projectile position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl;
              */
}

void Projectile::setPierce(int p) {
    pierce = p;
}

void Projectile::reducePierce(int amount) {
    pierce -= amount;
    if (pierce < 0) {
        pierce = 0;
    }
}

int Projectile::getPierce() {
    return pierce;
}
std::vector<Enemy*> Projectile::getPiercedEnemies() {
    return piercedEnemies;
}

void Projectile::addPiercedEnemy(Enemy* enemy) {
	if (!enemy) return;
	if (std::find(piercedEnemies.begin(), piercedEnemies.end(), enemy) != piercedEnemies.end()) {
        return; // already pierced
    }
    piercedEnemies.push_back(enemy);
}

