#include "enemySpawner.h"
#include "enemy.h"
#include "entityLoader.h"
#include "constants.h"
#include <cstdlib>
#include <ctime>

EnemySpawner::EnemySpawner(Node* sceneRoot, glm::vec3 position, std::vector<Enemy*>& enemyList)
    : PhysicShapeObject(nullptr, position), timeSinceLastSpawn(0.0f), sceneRoot(sceneRoot), enemyList(enemyList) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed for randomness
}

void EnemySpawner::SpawnEnemy() {
    if (enemyList.size() >= Config::EnemySpawner::MAX_ENEMIES) {
        return;
    }
    float randValue = static_cast<float>(std::rand()) / RAND_MAX;
    float cumulativeProbability = 0.0f;
    int tier = 1;
    for (size_t i = 0; i < spawnProbabilities.size(); ++i) {
        cumulativeProbability += spawnProbabilities[i];
        if (randValue <= cumulativeProbability) {
            tier = static_cast<int>(i) + 1;
            break;
        }
    }

    // random position within radius around spawner position
    float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * 3.14159265f;
    float random01 = static_cast<float>(std::rand()) / RAND_MAX;
    float distance = std::sqrt(random01) * (radius-noSpawnRadius) + noSpawnRadius;

    glm::vec3 offset = glm::vec3(std::cos(angle) * distance, 0.0f, std::sin(angle) * distance);

    glm::vec3 finalPos = Position + offset;

    Enemy* newEnemy = EntityLoader::CreateEnemy(finalPos, tier);

    enemyList.push_back(newEnemy);
    sceneRoot->add(newEnemy);
}

void EnemySpawner::updateSpawnProbabilities(int playerLevel) {
    
    float x = (float)(playerLevel - 1); 
    float w1 = 1000.0f; 
    float w2 = 100.0f + (1000.0f * x);
    float w3 = 50.0f + (400.0f * std::pow(x, 2));
    float w4 = 10.0f + (100.0f * std::pow(x, 3));
    float totalWeight = w1 + w2 + w3 + w4;

    if (totalWeight > 0.0f) {
        spawnProbabilities[0] = w1 / totalWeight;
        spawnProbabilities[1] = w2 / totalWeight;
        spawnProbabilities[2] = w3 / totalWeight;
        spawnProbabilities[3] = w4 / totalWeight;
    }
}

void EnemySpawner::Update(float deltaTime) {
    timeSinceLastSpawn += deltaTime;

    if (timeSinceLastSpawn >= Config::EnemySpawner::SPAWN_INTERVAL) {
        SpawnEnemy();
        timeSinceLastSpawn = 0.0f;
    }
}

