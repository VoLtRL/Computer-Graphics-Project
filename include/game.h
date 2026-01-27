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
#include "textRenderer.h"
#include "statsMenu.h"

class Game {
public:
    Game(Viewer* viewer);
    ~Game();

    void Init();
    
    void Update();
    void RenderUI();
    void RenderDeathUI();
    void RenderWinUI();
    
    Player* getPlayer() const { return player; }
    bool getHasWon() const { return hasWon; }
    void setHasWon(bool won) { hasWon = won; }

    void ProcessInput(float deltaTime);

private:
    Viewer* viewer;
    Sprite* spriteRenderer;
    TextRenderer* textRenderer;

    Player* player;
    StatsMenu* statsMenu;
    std::vector<Enemy*> enemies;
    std::vector<EnemySpawner*> enemySpawners;
    Map* map;
    Crosshair* crosshair;
    HandlePhysics* handlePhysics;

    bool isTimeRecorded = false;
    double timeRecorded = 0.0;

    unsigned int gameOverTexture;
    unsigned int victoryTexture;
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
    bool hasWon = false;

};