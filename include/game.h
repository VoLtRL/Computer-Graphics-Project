#pragma once

#include "viewer.h"
#include "resourceManager.h"
#include "handlePhysics.h"
#include "player.h"
#include "map.h"
#include "crosshair.h"
#include "enemy.h"
#include <vector>
#include "sprite.h"
#include "enemySpawner.h"
#include "constants.h"

class Game {
public:
    Game(Viewer* viewer);
    ~Game();

    void Init();
    
    void Update();
    void RenderUI();

    void ProcessInput(float deltaTime);

    std::map<std::string, float> lootTable{ {"", 50.0f}, {"DamageBoost", 10.0f }, {"SpeedBoost", 10.0f}, {"HealthPack", 20.0f},{"Fear", 10.0f}};

private:
    Viewer* viewer;
    Sprite* spriteRenderer;


    Player* player;
    std::vector<Enemy*> enemies;
    std::vector<EnemySpawner*> enemySpawners;
    Map* map;
    Crosshair* crosshair;
    HandlePhysics* handlePhysics;

    unsigned int gameOverTexture;
    unsigned int healthBarTexture;
    unsigned int crosshairTexture;

    glm::vec4 fogColor = Config::Game::fogColor;
    float fogStart = Config::Game::fogStartDistance;
    float fogEnd = Config::Game::fogEndDistance;

    GLint fogColorLocation;
    GLint fogStartLocation;
    GLint fogEndLocation;

    glm::vec3 skyColor = glm::vec3(0.5f, 0.5f, 0.5f);

    int enemyKilled = 0;

};