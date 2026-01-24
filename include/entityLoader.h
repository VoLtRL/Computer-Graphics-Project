#pragma once
#include <glm/glm.hpp>
#include "player.h"
#include "enemy.h"
#include "physicShapeObject.h"

class Shader;

class EntityLoader {
public:

    static Player* CreatePlayer(glm::vec3 position);
    static Projectile* CreateProjectile(glm::vec3 pos, glm::vec3 dir, Player* shooter);
    static Enemy* CreateEnemy(glm::vec3 position);
    static PhysicShapeObject* CreateTestBox(glm::vec3 position);
};