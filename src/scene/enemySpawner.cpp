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
    if (enemyCount >= Config::EnemySpawner::MAX_ENEMIES) {
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
    float distance = static_cast<float>(std::rand()) / RAND_MAX * radius;

    glm::vec3 offset = glm::vec3(std::cos(angle) * distance, 0.0f, std::sin(angle) * distance);

    glm::vec3 finalPos = this->Position + offset;

    Enemy* newEnemy = EntityLoader::CreateEnemy(finalPos, tier);

    enemyList.push_back(newEnemy);
    sceneRoot->add(newEnemy);

    std::cout << "Spawned Enemy of Tier " << tier << " at position (" 
              << finalPos.x << ", " << finalPos.y << ", " << finalPos.z << ")\n";
    enemyCount++;
}

void EnemySpawner::Update(float deltaTime) {
    timeSinceLastSpawn += deltaTime;

    if (timeSinceLastSpawn >= Config::EnemySpawner::SPAWN_INTERVAL) {
        SpawnEnemy();
        timeSinceLastSpawn = 0.0f;
    }
}

