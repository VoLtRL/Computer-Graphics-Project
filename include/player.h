#pragma once
#include "physicShapeObject.h"
#include <glm/glm.hpp>

// Forward declaration pour éviter les inclusions circulaires si nécessaire
class Weapon; 

class Player : public PhysicShapeObject {
public:
    Player(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f));

    // game loop update
    void update(float deltaTime); 

    // actions
    void jump();
    void attack();
    void move(glm::vec3 direction);

    // health management
    void takeDamage(float damage);
    void heal(float amount);
    
    // getters
    float getHealth() const { return health; }
    bool isAlive() const { return health > 0.0f; }
    float getSpeed() const { return movementSpeed; }
    float getSize() const { return size; }

    // resizing
    void resize(float scale);

    // internal updates
    void updateAnimation(float deltaTime);
    void updateOrientation();

    // setters
    void gainJumpStrength(float quantity);

private:
    // stats
    float health;
    float maxHealth;
    float movementSpeed;
    float jumpStrength;
    float attackDamage;
    float attackSpeed; 
    float size;

    // states
    bool isJumping;
    float attackCooldown;

    // temp
    float groundDamping;


};