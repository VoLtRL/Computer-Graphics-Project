#include "enemy.h"
#include "player.h"

Enemy::Enemy(Shape* shape, glm::vec3 position, Shader* projectileShader)
    : PhysicShapeObject(shape, position), health(100), power(10) {
}

Enemy::~Enemy() {
    delete this;
}

void Enemy::moveTowardsPlayer(glm::vec3 playerPosition, float deltaTime) {
    glm::vec3 direction = glm::normalize(playerPosition - this->Position);
    this->Position += direction * speed * deltaTime; 
}

void Enemy::attack(Player* player , float deltaTime) {
    if (health > 0 && player->isAlive() && attackCooldown <= 0.0f) {
        player->takeDamage(power);
        attackCooldown = 1.0f / attackSpeed;
    }else{
        attackCooldown -= deltaTime;
    }
}