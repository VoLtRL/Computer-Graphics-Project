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
      attackCooldown(0.0f)
{
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

    // handle animations
    updateAnimation(deltaTime);

    // handle orientation
    updateOrientation();
}

void Player::draw(glm::mat4& view, glm::mat4& projection)
{
    PhysicShapeObject::draw(view, projection);
}

void Player::jump()
{
    if (!isJumping) {
        Velocity.y += jumpStrength;
        isJumping = true;
    }
}

void Player::attack()
{
    if (attackCooldown <= 0.0f) {
        attackCooldown = 1.0f / attackSpeed;
    }
}

void Player::move(glm::vec3 direction)
{
    glm::vec3 normDir = glm::normalize(direction);
    Position += normDir * movementSpeed;
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

