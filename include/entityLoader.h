#pragma once
#include <glm/glm.hpp>
#include "player.h"
#include "enemy.h"
#include "physicShapeObject.h"
#include "enemySpawner.h"

class Shader;

class EntityLoader {
public:

    static Player* CreatePlayer(glm::vec3 position);
    static Projectile* CreateProjectile(glm::vec3 pos, glm::vec3 dir, Player* shooter);
    static Enemy* CreateEnemy(glm::vec3 position,int tier);
    static PhysicShapeObject* CreateTestBox(glm::vec3 position);
    static EnemySpawner* CreateEnemySpawner(Node* sceneRoot, glm::vec3 position, std::vector<Enemy*>& enemyList);
};