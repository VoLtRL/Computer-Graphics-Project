#pragma once
#include "shader.h"
#include "physicShapeObject.h"
#include "projectile.h"
#include <glm/glm.hpp>
#include <vector>

// Forward declaration pour éviter les inclusions circulaires si nécessaire
class Weapon; 

class Player : public PhysicShapeObject {
public:
    Player(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f), Shader* projectileShader = nullptr);

    // game loop update
    void update(float deltaTime); 
    void draw(glm::mat4& view, glm::mat4& projection) override;

    // actions
    void jump();
    void shoot(glm::vec3 shootDirection);
    void move(glm::vec3 direction);

    // health management
    void takeDamage(float damage);
    void heal(float amount);
    
    // getters
    float getHealth() const { return health; }
    bool isAlive() const { return health > 0.0f; }
    float getSpeed() const { return movementSpeed; }
    float getSize() const { return size; }
    std::vector<Projectile*> getActiveProjectiles() const { return activeProjectiles; }

    // resizing
    void resize(float scale);

    // internal updates
    void updateAnimation(float deltaTime);
    void updateOrientation();

    // setters
    void gainJumpStrength(float quantity);

	void BeforeCollide(PhysicObject* other, CollisionInfo info) override;
	void OnCollide(PhysicObject* other, CollisionInfo info) override;


	// Position and velocity register
    glm::vec3 PreviousPosition;
	glm::vec3 PreviousVelocity;

private:
    // stats
    float health;
    float maxHealth;
    float movementSpeed;
    float jumpStrength;
    float attackDamage;
    float attackSpeed; 
    float size;
    float projectileSpeed;

    // states
    bool isJumping;
    float attackCooldown;

    // temp
    float groundDamping;

    // projectile shader
    Shader* projectileShader;
    // projectile storage
    std::vector<Projectile*> activeProjectiles;


};