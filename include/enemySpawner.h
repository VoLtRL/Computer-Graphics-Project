#pragma once
#include "enemy.h"
#include "physicShapeObject.h"
#include "node.h"

class EnemySpawner : public PhysicShapeObject {
    public:
        EnemySpawner(Node* sceneRoot, glm::vec3 position, std::vector<Enemy*>& enemyList);
        
        void SpawnEnemy();

        void Update(float deltaTime);

    private:
        inline static int enemyCount = 0;
        inline static std::vector<float> spawnProbabilities = {1.0f, 0.0f, 0.00f, 0.0f};
        float timeSinceLastSpawn;
        Node* sceneRoot;
        std::vector<Enemy*>& enemyList;
        float radius = 2.0f;
                
};