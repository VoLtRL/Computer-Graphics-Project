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
      attackSpeed(2.0f),
      size(1.0f),
      isJumping(false),
      attackCooldown(0.0f),
      groundDamping(8.0f),
      projectileSpeed(30.0f),
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
    if (Position.y <= 0.5f) {
        Velocity.x -= Velocity.x * glm::exp(-groundDamping * deltaTime);
        Velocity.z -= Velocity.z * glm::exp(-groundDamping * deltaTime);
        isJumping = false;
    }   

    // debug info
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
}

void Player::draw(glm::mat4& view, glm::mat4& projection)
{
    if (this->model != nullptr) {

        glm::mat4 model = glm::mat4(1.0f);
        
        // Position
        model = glm::translate(model, this->Position);
        //For good positioning on the platform
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

        // Orientation
        glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), this->FrontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
        model = model * rotation;

        // Ajustement
        // model = glm::scale(model, glm::vec3(0.5f));
        // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0,1,0));

        // Draw
        this->model->draw(model, view, projection);
    } 
    else {//Backup
        PhysicShapeObject::draw(view, projection);
    }

    // Projectiles
    for (auto p : activeProjectiles) {
        p->draw(view, projection);
    }
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

void Player::shoot(glm::vec3 shootDirection){

    if(attackCooldown <= 0.0f){
        
        glm::vec3 shootingOrigin = Position + (UpVector * 1.2f) + (FrontVector * 0.5f);

        float spawnDistance = 0.5f;
        glm::vec3 spawnPos = shootingOrigin + (shootDirection * spawnDistance);

        Shape* proj_shape = new Sphere(projectileShader, size * 0.2f, 20);
        proj_shape->color = glm::vec3(1.0f, 1.0f, 1.0f);
        proj_shape->isEmissive = true;

        Projectile* proj = new Projectile(proj_shape, spawnPos, projectileSpeed, attackDamage, 50.0f);

        proj->Velocity = shootDirection * projectileSpeed;

        proj->FrontVector = shootDirection;
        proj->RightVector = glm::normalize(glm::cross(proj->FrontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
        proj->UpVector    = glm::normalize(glm::cross(proj->RightVector, proj->FrontVector));

        activeProjectiles.push_back(proj);
        attackCooldown = 1.0f / attackSpeed;
    }
}

void Player::move(glm::vec3 direction)
{
    if(!isJumping){
        glm::vec3 normDir = glm::normalize(direction);
        // Update velocity
        Velocity.x = normDir.x * movementSpeed;
        Velocity.z = normDir.z * movementSpeed;
        // Update orientation vectors
        FrontVector = glm::normalize(glm::vec3(normDir.x, 0.0f, normDir.z));
        RightVector = glm::normalize(glm::cross(FrontVector, WorldUpVector));
        UpVector = glm::normalize(glm::cross(RightVector, FrontVector));
        
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

