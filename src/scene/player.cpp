#include "player.h"
#include "projectile.h"
#include "sphere.h"

#include <vector>
#include <GLFW/glfw3.h>

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

void Player::draw(glm::mat4& view, glm::mat4& projection){

    glm::mat4 model = glm::mat4(1.0f);
    
    // Position
    model = glm::translate(model, this->Position);
    //For good positioning on the platform
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));

    // Orientation
    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), this->FrontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
    model = model * rotation;

    // Draw
    this->model->draw(model, view, projection);

    // Projectiles ?
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
    if (!model) return;

    //Player moving check
    bool isMoving = glm::length(glm::vec2(Velocity.x, Velocity.z)) > 0.1f;

    if (isMoving) {

        float speed = 10.0f; 
        float time = glfwGetTime();
        
        float angle = glm::sin(time * speed) * 0.8f; 

        // Rotation + base
        if (armLeft)  armLeft->set_transform(glm::rotate(armLeftOrig, angle, glm::vec3(1, 0, 0)));
        if (armRight) armRight->set_transform(glm::rotate(armRightOrig, -angle, glm::vec3(1, 0, 0)));
        if (legLeft)  legLeft->set_transform(glm::rotate(legLeftOrig, -angle, glm::vec3(1, 0, 0)));
        if (legRight) legRight->set_transform(glm::rotate(legRightOrig, angle, glm::vec3(1, 0, 0)));
    } 
    else {
        // Reset
        if (armLeft)  armLeft->set_transform(armLeftOrig);
        if (armRight) armRight->set_transform(armRightOrig);
        if (legLeft)  legLeft->set_transform(legLeftOrig);
        if (legRight) legRight->set_transform(legRightOrig);
    }
}

// Recursive search for a node by name
Node* recursiveFind(Node* node, std::string name) {
    if (node->name.find(name) != std::string::npos) return node;
    for (auto child : node->children_) { 
        Node* res = recursiveFind(child, name);
        if (res) return res;
    }
    return nullptr;
}

void Player::setModel(Node* modelNode) {
    this->model = modelNode;

    //Find body parts
    armLeft = recursiveFind(model, "Arm_upper_L");
    armRight = recursiveFind(model, "Arm_upper_R");
    legLeft = recursiveFind(model, "Leg_upper_L");
    legRight = recursiveFind(model, "Leg_upper_R");

    //Save initial transforms
    if(armLeft) armLeftOrig = armLeft->get_transform();
    if(armRight) armRightOrig = armRight->get_transform();
    if(legLeft) legLeftOrig = legLeft->get_transform();
    if(legRight) legRightOrig = legRight->get_transform();
}
