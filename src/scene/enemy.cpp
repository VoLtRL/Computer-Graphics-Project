#include "enemy.h"
#include "player.h"
#include "projectile.h"

Enemy::Enemy(Shape* shape, glm::vec3 position, Shader* projectileShader)
    : PhysicShapeObject(shape, position), health(100), power(10) {
}

Enemy::~Enemy() {
    if (model) {
        delete model;
        model = nullptr;
    }
}

void Enemy::OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{

    Projectile* proj = dynamic_cast<Projectile*>(other);
    if (proj && info.hit) {
        this->takeDamage(proj->getDamage()); // call attack on enemy
        proj->deactivate();
    }
}

void Enemy::moveTowardsPlayer(glm::vec3 playerPosition, float deltaTime) {
    glm::vec3 direction = glm::normalize(playerPosition - this->Position);
    this->Position += direction * speed * deltaTime; 
}

void Enemy::setModel(Node* modelNode) {
    this->model = modelNode;
}

void Enemy::draw(glm::mat4& view, glm::mat4& projection) {
    if (model) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, Position);
        modelMatrix *= RotationMatrix;
        model->draw(modelMatrix, view, projection);
    }else{
        PhysicShapeObject::draw(view, projection);
    }
}

void Enemy::attack(Player* player , float deltaTime) {
    if (health > 0 && player->isAlive() && attackCooldown <= 0.0f) {
        player->takeDamage(power);
        attackCooldown = 1.0f / attackSpeed;
    }else{
        attackCooldown -= deltaTime;
    }
}

void Enemy::takeDamage(int damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
}