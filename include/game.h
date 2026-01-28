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
    void ProcessGameOverInput();

	std::map<std::string, double> getLootTableProbabilities(float alpha);
  
private:
    Viewer* viewer;
    Sprite* spriteRenderer;
    TextRenderer* textRenderer;

    Player* player = nullptr;
    StatsMenu* statsMenu;
    std::vector<Enemy*> enemies;
    EnemySpawner* enemySpawner;
    Map* map;
    Crosshair* crosshair;
    HandlePhysics* handlePhysics;

    bool isTimeRecorded;
    double timeRecorded;

    unsigned int gameOverTexture;
    unsigned int victoryTexture;
    unsigned int healthBarTexture;
    unsigned int crosshairTexture;
    unsigned int experienceBarTexture;

    glm::vec4 fogColor;
    float fogStart;
    float fogEnd;

    GLint fogColorLocation;
    GLint fogStartLocation;
    GLint fogEndLocation;

    glm::vec3 skyColor;

    int enemyKilled;
    bool hasWon;

    double resetGameTime = 0.0;

    std::map<std::string, float> lootTable{ {"", 1.0f}, {"DamageBoost", 4.0f }, {"SpeedBoost", 2.0f}, {"HealthPack", 2.5f},{"Fear", 5.5f} };


};