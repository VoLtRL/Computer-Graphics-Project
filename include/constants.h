#pragma once
#include <string>
#include <glm/glm.hpp>

// constants for the game
namespace Config {
    // window dimensions
    constexpr int SCR_WIDTH = 1920;
    constexpr int SCR_HEIGHT = 1080;

    // game constants
    namespace Game {
        constexpr int EnemiesToWin = 15;
    }
    
    // physics constants
    namespace Physics {
        constexpr float GRAVITY = -9.81f;
    }

    // player constants
    namespace Player {
        constexpr float MASS = 70.0f;
        constexpr float RADIUS = 0.5f;
        constexpr float JUMP_FORCE = 15.0f;
        constexpr float SPEED = 5.0f;
        constexpr glm::vec3 SPAWN_POS = glm::vec3(0.0f, 10.0f, 0.0f);
        constexpr glm::vec3 COLOR = glm::vec3(0.22f, 0.65f, 0.92f);
    }

    // enemy constants
    namespace Enemy {
        constexpr float MASS = 50.0f;
        constexpr float RADIUS = 0.5f;
        constexpr float SPEED = 2.0f;
        constexpr float ATTACK_SPEED = 1.0f;
        constexpr int POWER = 10;
        constexpr glm::vec3 COLOR = glm::vec3(0.9f, 0.1f, 0.1f);
    }

    namespace EnemySpawner {
        constexpr float SPAWN_INTERVAL = 0.1f;
        constexpr float SPAWN_RADIUS = 15.0f;

        constexpr int MAX_ENEMIES = 2000;
    }
}