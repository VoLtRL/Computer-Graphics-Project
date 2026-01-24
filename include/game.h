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

class Game {
public:
    Game(Viewer* viewer);
    ~Game();

    void Init();
    
    void Update();
    void RenderUI();

    void ProcessInput(float deltaTime);

private:
    Viewer* viewer;
    Sprite* spriteRenderer;


    Player* player;
    std::vector<Enemy*> enemies;
    Map* map;
    Crosshair* crosshair;
    HandlePhysics* handlePhysics;

    unsigned int gameOverTexture;
    unsigned int healthBarTexture;
    unsigned int crosshairTexture;

};