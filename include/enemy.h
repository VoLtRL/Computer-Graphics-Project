#pragma once
#include "physicShapeObject.h"
#include "player.h"
#include "node.h"

class Enemy : public PhysicShapeObject { 
public:
    Enemy(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f), Shader* projectileShader = nullptr);
    ~Enemy();
    void attack(Player* player, float deltaTime);
    void moveTowardsPlayer(glm::vec3 playerPosition, float deltaTime);

    void setModel(Node* modelNode);
    void draw(glm::mat4& view, glm::mat4& projection) override;

    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getSpeed() const { return speed; }

    void setAttackSpeed(float newAttackSpeed) { attackSpeed = newAttackSpeed; }
    float getAttackSpeed() const { return attackSpeed; }

    void setPower(int newPower) { power = newPower; }
    int getPower() const { return power; }

    float getHealth() const { return health; }
    void setHealth(int newHealth) { health = newHealth; }
    
    void takeDamage(int damage);
    bool isAlive() const { return health > 0; }
    int getTier() const { return tier; }
    void setTier(int newTier) { tier = newTier; }

    void setExperienceReward(float exp) { experienceReward = exp; }
    float getExperienceReward() const { return experienceReward; }

    void OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime) override;

private:
    int health;
    int power;
    int defensePower;
    float speed;
    float attackCooldown;
    float attackSpeed;
    int tier;
    float experienceReward;
    
    Node* model = nullptr;

 };