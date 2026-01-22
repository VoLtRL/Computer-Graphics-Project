#pragma once
#include "physicShapeObject.h"
#include "player.h"

class Enemy : public PhysicShapeObject { 
public:
    Enemy(Shape* shape = nullptr, glm::vec3 position = glm::vec3(0.0f), Shader* projectileShader = nullptr);
    ~Enemy();
    void attack(Player* player, float deltaTime);
    void moveTowardsPlayer(glm::vec3 playerPosition, float deltaTime);

    void setSpeed(float newSpeed) { speed = newSpeed; }
    float getSpeed() const { return speed; }

    void setAttackSpeed(float newAttackSpeed) { attackSpeed = newAttackSpeed; }
    float getAttackSpeed() const { return attackSpeed; }

    void setPower(int newPower) { power = newPower; }
    int getPower() const { return power; }

    float getHealth() const { return health; }
    void takeDamage(int damage) { health -= damage; if (health < 0) health = 0; }
    bool isAlive() const { return health > 0; }

private:
    int health;
    int power;
    int defensePower;
    float speed;
    float attackCooldown;
    float attackSpeed;
    
 };