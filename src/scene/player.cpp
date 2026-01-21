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

void Player::OnCollide(PhysicObject* other, CollisionInfo info)
{
    // Simple ground collision detection
    if (isJumping && info.hit && PhysicObject::Length2(PreviousPosition-Position) < 0.05f) {
        isJumping = false; // Reset jumping state when colliding with ground
    }
    else {
        //std::cout << PhysicObject::Length2(PreviousPosition - Position) << std::endl;
    }
}

void Player::update(float deltaTime)
{

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
        }
    }

    // handle animations
    updateAnimation(deltaTime);
}

void Player::draw(glm::mat4& view, glm::mat4& projection)
{
    // Draw the player using the PhysicShapeObject's draw method
    PhysicShapeObject::draw(view, projection);
    // Draw active projectiles
    for(Projectile* proj : activeProjectiles){
        if(proj->isActive()){
            proj->draw(view, projection);
        }
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

        Shape* proj_shape = new Sphere(projectileShader, size * 0.2f, 20);

        proj_shape->color = glm::vec3(1.0f, 0.96f, 0.86f);
        proj_shape->isEmissive = true;

        Projectile* proj = new Projectile(proj_shape, spawnPos, projectileSpeed, attackDamage, 50.0f);
        proj->Velocity = shootDirection * projectileSpeed;
        proj->SetMass(20.0f);
        proj->kinematic = false;
        proj->collisionShape = proj_shape;
        proj->shapeType = SPHERE;
        proj->canCollide = true;

        proj->setFrontVector(shootDirection);
        proj->setRightVector(glm::normalize(glm::cross(proj->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
        proj->setUpVector(glm::normalize(glm::cross(proj->GetRightVector(), proj->GetFrontVector())));
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

void Player::resize(float scale)
{
    size *= scale;
}

void Player::updateAnimation(float deltaTime)
{
    // TODO
}

