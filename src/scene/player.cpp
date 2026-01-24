#include "player.h"
#include "projectile.h"
#include "sphere.h"
#include "enemy.h"
#include "entityLoader.h"

#include <vector>
#include <GLFW/glfw3.h>

Player::Player(Shape* shape, glm::vec3 position,Shader* projectileShader)
    : PhysicShapeObject(shape, position),
      health(100.0f),
      maxHealth(100.0f),
      movementSpeed(5.0f),
      jumpStrength(10.0f),
      attackDamage(20.0f),
      attackSpeed(50.0f),
      size(1.0f),
      isJumping(false),
      attackCooldown(0.0f),
      groundDamping(8.0f),
      projectileSpeed(30.0f),
      projectileShader(projectileShader),
      PreviousPosition(position)

{
    std::cout << "Player created at position: (" 
              << position.x << ", " 
              << position.y << ", " 
              << position.z << ")" << std::endl;
}

void Player::BeforeCollide(PhysicObject* other, CollisionInfo info)
{
    // Placeholder for any pre-collision logic
    if (info.hit) {
        PreviousPosition = Position;
        PreviousVelocity = Velocity;
	}
}

void Player::OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{
    bool isGround = glm::dot(this->GetUpVector(), info.normal) > 0.7f;
    if (isJumping && info.hit && isGround && PhysicObject::Length2(PreviousPosition-Position) < 0.05f) {
        isJumping = false; // Reset jumping state when colliding with ground
    }
    Enemy* enemy = dynamic_cast<Enemy*>(other);
    if (enemy && info.hit) {
        enemy->attack(this, deltaTime); // call attack on enemy
    }
    else {
        //std::cout << PhysicObject::Length2(PreviousPosition - Position) << std::endl;
    }
}

void Player::update(float deltaTime)
{
    if (isDead) {
        if (model && deathTimer == 0.0f) {
            glm::mat4 current = model->get_transform();
            model->set_transform(glm::rotate(current, glm::radians(90.0f), glm::vec3(1,0,0)));
        }
        deathTimer += deltaTime;
        return;
    }

    if (health <= 0.0f) {
        die();
        return;
    }

    // handle attack cooldown
    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
        if (attackCooldown < 0.0f) {
            attackCooldown = 0.0f;
        }
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
        } else {
            deleteActiveProjectile(proj);
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
    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), this->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f)));
    model = model * rotation;

    // Scale
    model = glm::scale(model, glm::vec3(this->size));

    // Draw
    this->model->draw(model, view, projection);

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
        
        glm::vec3 shootingOrigin = Position + (GetUpVector() * 1.2f) + (GetFrontVector() * 0.5f);

        float spawnDistance = 0.5f;
        glm::vec3 spawnPos = shootingOrigin + (shootDirection * spawnDistance);

        Projectile* proj = EntityLoader::CreateProjectile(spawnPos, shootDirection, this);

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
        glm::vec3 FrontVector = glm::normalize(glm::vec3(normDir.x, 0.0f, normDir.z));
        glm::vec3 RightVector = glm::normalize(glm::cross(FrontVector, WorldUpVector));
        glm::vec3 UpVector = glm::normalize(glm::cross(RightVector, FrontVector));
		RotationMatrix = glm::mat4(1.0f);
		RotationMatrix[0] = glm::vec4(RightVector, 0.0f);
		RotationMatrix[1] = glm::vec4(UpVector, 0.0f);
		RotationMatrix[2] = glm::vec4(-FrontVector, 0.0f);
        
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

void Player::die() {
    if (!isDead) {
        isDead = true;
        std::cout << "GAME OVER" << std::endl;
    }
}

void Player::resize(float scale)
{
    size *= scale;
    
    if (collisionShape && shapeType == ShapeType::ST_SPHERE) {
        ((Sphere*)collisionShape)->radius *= scale;
    }
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

void Player::deleteActiveProjectile(Projectile* proj){
    auto it = std::find(activeProjectiles.begin(), activeProjectiles.end(), proj);
    if (it != activeProjectiles.end()) {
        activeProjectiles.erase(it);
    }
    PhysicObject::deleteObject(proj);
}

