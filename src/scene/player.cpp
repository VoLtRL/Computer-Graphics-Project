#include "player.h"
#include "projectile.h"
#include "sphere.h"

#include <vector>

Player::Player(Shape* shape, glm::vec3 position,Shader* projectileShader)
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
      groundDamping(8.0f),
      projectileSpeed(25.0f),
      projectileShader(projectileShader)
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

    // projectile handling
    for(Projectile* proj : activeProjectiles){
        if(proj->isActive()){
            proj->update(deltaTime);
        }
    }

    // handle animations
    updateAnimation(deltaTime);

    // handle orientation
    updateOrientation();
}

void Player::jump(){
    if(!isJumping){
        Velocity.y += jumpStrength; // basic jump impulse
        isJumping = true; // set jumping state
    }
}

void Player::gainJumpStrength(float quantity){
    jumpStrength += quantity;
}

void Player::shoot(){
    if(attackCooldown <= 0.0f){
        // Create and launch projectile
        Shape* proj_shape = new Sphere(projectileShader, size * 0.2f, 8);
        Projectile* proj = new Projectile(proj_shape, Position + FrontVector * (size + 0.2f), projectileSpeed, attackDamage, 50.0f);
        activeProjectiles.push_back(proj);
        attackCooldown = 1.0f / attackSpeed; // Reset cooldown
    }else{
        std::cout << "Attack on cooldown: " << attackCooldown << " seconds remaining." << std::endl;
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

