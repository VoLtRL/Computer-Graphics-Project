#include "player.h"

Player::Player(Shape* shape, glm::vec3 position)
    : PhysicShapeObject(shape, position),
      health(100.0f),
      maxHealth(100.0f),
      movementSpeed(5.0f),
      jumpStrength(10.0f),
      attackDamage(20.0f),
      attackSpeed(1.0f),
      size(1.0f),
      isJumping(false),
      attackCooldown(0.0f),
      groundDamping(8.0f)
{
    std::cout << "Player created at position: (" 
              << position.x << ", " 
              << position.y << ", " 
              << position.z << ")" << std::endl;
}

void Player::update(float deltaTime)
{
    // handle physics
    UpdatePhysics(deltaTime);

    // handle attack cooldown
    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
        if (attackCooldown < 0.0f) {
            attackCooldown = 0.0f;
        }
    }
    // handle ground damping
    if (Position.y <= 0.0f) {
        Velocity.x -= Velocity.x * glm::exp(-groundDamping * deltaTime);
        Velocity.z -= Velocity.z * glm::exp(-groundDamping * deltaTime);
        isJumping = false;
    }   

    std::cout << "Player position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl;
    std::cout << "is Jumping ? " 
              << isJumping << std::endl;

    // handle animations
    updateAnimation(deltaTime);

    // handle orientation
    updateOrientation();
}

void Player::jump(){
    if(!isJumping){
        Velocity.y += jumpStrength; // Impulse based on jump strength and mass
        isJumping = true;
    }
}

void Player::gainJumpStrength(float quantity){
    jumpStrength += quantity;
}

void Player::attack(){
    if (attackCooldown <= 0.0f) {
        attackCooldown = 1.0f / attackSpeed;
    }
}

void Player::move(glm::vec3 direction)
{
    if(!isJumping){
        glm::vec3 normDir = glm::normalize(direction);
        Velocity.x = normDir.x * movementSpeed;
        Velocity.z = normDir.z * movementSpeed;
    }
}

void Player::takeDamage(float damage)
{
    health -= damage;
    if (health < 0.0f) {
        health = 0.0f;
    }
}

void Player::heal(float amount)
{
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

void Player::resize(float scale)
{
    size *= scale;
}

void Player::updateAnimation(float deltaTime)
{
    // TODO
}

void Player::updateOrientation()
{
    // TODO
}

