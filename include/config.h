#pragma once
#include "shape.h"
#include <glm/glm.hpp>

struct Config {
    const unsigned int SCR_WIDTH = 1280;
    const unsigned int SCR_HEIGHT = 720;
    const float MOUSE_SENSITIVITY = 0.1f;
    const float FOV = 45.0f;
    const float NEAR_PLANE = 0.1f;
    const float FAR_PLANE = 100.0f;
};

struct PlayerConfig {
    glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    Shape* playerShape = nullptr;

    glm::vec3 color = glm::vec3(0.22f, 0.65f, 0.92f);
    bool useCheckerboard = false;

    float mass = 70.0f;
    float maxHealth = 100.0f;
    float movementSpeed = 5.0f;
    float jumpStrength = 10.0f;
    float attackDamage = 20.0f;
    float attackSpeed = 50.0f; 
    float size = 1.0f;
    float projectileSpeed = 30.0f;
    float Damping = 0.8f;
    float Friction = 1.0f;
    Shape* collisionShape = nullptr;
    ShapeType shapeType = SPHERE;
    bool canCollide = true;
    std::string name = "Player";
};

struct EnemyConfig {
    int health = 100;
    int power = 10;
    float speed = 3.0f;
    float attackSpeed = 1.0f; 
    float Damping = 1.0f;
    float Friction = 1.0f;
    Shape* collisionShape = nullptr;
    ShapeType shapeType = SPHERE;
    bool canCollide = true;
    std::string name = "Enemy";
};

struct ProjectileConfig {
    float speed = 30.0f;
    float damage = 20.0f;
    float lifetime = 100.0f;
    float Damping = 0.0f;
    float Friction = 0.0f;
    Shape* collisionShape = nullptr;
    ShapeType shapeType = SPHERE;
    bool canCollide = true;
    std::string name = "Projectile";
};