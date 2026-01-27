#pragma once
#include "physicShapeObject.h"
#include <glm/glm.hpp>

// Forward declaration pour éviter les inclusions circulaires si nécessaire
class Weapon; 

class Player : public PhysicShapeObject {
public:
    Player(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f));

    // game loop update
    void update(float deltaTime); 

    // actions
    void jump();
    void attack();
    void move(glm::vec3 direction);

    // health management
    void takeDamage(float damage);
    void heal(float amount);
    
    // getters
    float getHealth() const { return health; }
    bool isAlive() const { return health > 0.0f; }
    float getSpeed() const { return movementSpeed; }
    float getSize() const { return size; }

    // resizing
    void resize(float scale);

    // internal updates
    void updateAnimation(float deltaTime);
    void updateOrientation();

    // setters
    void gainJumpStrength(float quantity);

private:
    //Model
    Node* model = nullptr;

    // body parts for animation
    Node* torso = nullptr;
    Node* head = nullptr;
    Node* armLeft = nullptr;
    Node* armRight = nullptr;
    Node* legLeft = nullptr;
    Node* legRight = nullptr;
    Node* armLowerLeft = nullptr;
    Node* armLowerRight = nullptr;
    Node* legLowerLeft = nullptr;
    Node* legLowerRight = nullptr;
    
    // original transforms for resetting after animation
    glm::mat4 torsoOrig, headOrig;
    glm::mat4 armLeftOrig, armRightOrig;
    glm::mat4 legLeftOrig, legRightOrig;
    glm::mat4 armLowerLeftOrig, armLowerRightOrig;
    glm::mat4 legLowerLeftOrig, legLowerRightOrig;

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
    bool isDead = false;
    float deathTimer = 0.0f;

    // temp
    float groundDamping;

    // projectile shader
    Shader* projectileShader;
    // projectile storage
    std::vector<Projectile*> activeProjectiles;

    // animation
    float animTime = 0.0f;
    float currentLegAngle = 0.0f;
    float currentArmAngle = 0.0f;
    float currentLean = 0.0f;
    float currentTilt = 0.0f;
    float currentElbow = 0.0f;
    float currentKnee = 0.0f;
    float currentDrop = 0.0f;

    float landingImpact = 0.0f;
    bool wasInAir = false;

    glm::vec3 movementReferenceForward = glm::vec3(0, 0, 1);

    const float WALK_FREQ = 10.0f;
    const float SWING_AMP = 0.8f;

    // helper function to find nodes by name
    Node* findNode(Node* current, std::string targetName) {
        if (current->name == targetName) return current;
            return nullptr;
    }
};
