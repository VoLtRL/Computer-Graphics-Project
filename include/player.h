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
    
    // health getters/setters
    void setMaxHealth(float newMaxHealth) { maxHealth = newMaxHealth; }
    float getHealth() const { return health; }
    void setHealth(float newHealth) { health = newHealth; }
    float getMaxHealth() const { return maxHealth; }
    bool isAlive() const { return health > 0.0f; }

    // speed setter/getter
    void setSpeed(float speed) { movementSpeed = speed; }
    float getSpeed() const { return movementSpeed; }

    // size getter/setter
    float getSize() const { return size; }
    void setSize(float newSize) { size = newSize; }
    void resize(float scale);

    // projectile setter/getter
    std::vector<Projectile*> getActiveProjectiles() const { return activeProjectiles; }
    float getProjectileSpeed() const { return projectileSpeed; }
    // attack getter/setter
    float getAttackDamage() const { return attackDamage; }
    void setAttackDamage(float damage) { attackDamage = damage; }
    float getAttackSpeed() const { return attackSpeed; }
    void setAttackSpeed(float speed) { attackSpeed = speed; }
    // jump strength getter/setter
    float getJumpStrength() const { return jumpStrength; }
    void setJumpStrength(float strength) { jumpStrength = strength; }
    //level/xp getters/setters
    float setExperience(float xp) { experience = xp; }
    float getExperience() const { return experience; }
    void addExperience(float xp) { experience += xp; }
    void setExperienceToNextLevel(float ETNL) { experienceToNextLevel = ETNL; }
    float getExperienceToNextLevel() const { return experienceToNextLevel; }
    void setLevel(int lvl) {level = lvl;};
    int getLevel() const { return level; }
    void addLevel() { level += 1; }
    
    // internal updates
    void updateAnimation(float deltaTime);

	void BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;
	void OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;

    void deleteActiveProjectile(Projectile* proj);

    void levelUp();

	// Position and velocity register
    glm::vec3 PreviousPosition;
	glm::vec3 PreviousVelocity;

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
    float experience;
    float experienceToNextLevel;
    int level;

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

    float lastShootTime = -10.0f;
    float recoilForce = 0.0f;
    float combatBlend = 0.0f;

    // helper function to find nodes by name
    Node* recursiveFind(Node* node, std::string name) {
    if (node->name.find(name) != std::string::npos) return node;
    for (auto child : node->children_) { 
        Node* res = recursiveFind(child, name);
        if (res) return res;
    }
    return nullptr;
}
};

