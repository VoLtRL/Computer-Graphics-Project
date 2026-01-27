#pragma once
#include "shader.h"
#include "physicShapeObject.h"
#include "projectile.h"
#include <glm/glm.hpp>
#include <vector>

#include "node.h"

// Forward declaration pour éviter les inclusions circulaires si nécessaire
class Weapon; 

class Player : public PhysicShapeObject {
public:
    Player(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f), Shader* projectileShader = nullptr);

    // game loop update
    void update(float deltaTime); 
    void draw(glm::mat4& view, glm::mat4& projection) override;

    //Skin 3D
    void setModel(Node* modelNode);

    // actions
    void jump();
    void shoot(glm::vec3 shootDirection);
    void move(glm::vec3 direction);

    // health management
    void takeDamage(float damage);
    void heal(float amount);
    void die();
    
    // getters
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    void setHealth(float newHealth) { health = newHealth; }
    bool isAlive() const { return health > 0.0f; }
    float getSpeed() const { return movementSpeed; }
    float getSize() const { return size; }
    std::vector<Projectile*> getActiveProjectiles() const { return activeProjectiles; }
    float getProjectileSpeed() const { return projectileSpeed; }
    float getAttackDamage() const { return attackDamage; }
    float getAttackSpeed() const { return attackSpeed; }
    float getJumpStrength() const { return jumpStrength; }

    // resizing
    void resize(float scale);

    // internal updates
    void updateAnimation(float deltaTime);

    // setters
    void gainJumpStrength(float quantity);

	void BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
	void OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;

    void deleteActiveProjectile(Projectile* proj);


	// Position and velocity register
    glm::vec3 PreviousPosition;
	glm::vec3 PreviousVelocity;

private:
    //Model
    Node* model = nullptr;

    // body parts for animation
    Node* armLeft = nullptr;
    Node* armRight = nullptr;
    Node* legLeft = nullptr;
    Node* legRight = nullptr;
    
    // original transforms for resetting after animation
    glm::mat4 armLeftOrig;
    glm::mat4 armRightOrig;
    glm::mat4 legLeftOrig;
    glm::mat4 legRightOrig;

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
    bool canJump;
    float attackCooldown;
    bool isDead = false;
    float deathTimer = 0.0f;

    // temp
    float groundDamping;

    // projectile shader
    Shader* projectileShader;
    // projectile storage
    std::vector<Projectile*> activeProjectiles;

    // helper functions
    Node* findNode(Node* current, std::string targetName) {
        if (current->name == targetName) return current;
            return nullptr;
    }
};