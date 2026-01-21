#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm> // Added for std::find in destructor

class physicShapeObject; // Forward declaration

struct CollisionInfo {
    bool hit = false;
    glm::vec3 normal = glm::vec3(0.0f);
    float penetration = 0.0f;
};

struct OBBCollision {
    glm::vec3 center;
    glm::vec3 halfExtents; // (w/2, h/2, d/2)
    glm::mat3 rotation;    // orientation
};

struct SphereCollision {
    glm::vec3 center;
    float radius;
};

class Shape;

enum ShapeType {
    BOX,
    SPHERE,
    CAPSULE,
    INVALID
};

class PhysicObject {

public:
    PhysicObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    virtual ~PhysicObject(); // Added virtual destructor

    std::string name = "";

    // Position and movement
    glm::vec3 Position;
    glm::vec3 Velocity;
    glm::vec3 Acceleration;
    float Damping;
    float Friction;

    // Orientation vectors
    glm::mat4 RotationMatrix;

    inline static const glm::vec3 WorldUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

    // Mass
    float Mass;
    float InvMass;
    bool kinematic;
    inline static const float gravity = 9.8f;

    // Collisions
    bool canCollide;
    glm::vec3 forcesApplied;
    Shape* collisionShape;
    float Restitution;
    ShapeType shapeType;

    // Update physics state (Integration only)
    void UpdatePhysics(float deltaTime);

    void SetMass(float mass) {
        Mass = mass;
        InvMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
    }

    void ApplyForce(const glm::vec3& force) {
        forcesApplied += force;
    }

    glm::vec3 GetFrontVector() {
        return glm::vec3(RotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
    }

    glm::vec3 GetRightVector() {
        return glm::vec3(RotationMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
    }

    glm::vec3 GetUpVector() {
        return glm::vec3(RotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    }

    void setFrontVector(const glm::vec3& front) {
        RotationMatrix[2] = glm::vec4(-glm::normalize(front), 0.0f);
    }

    void setRightVector(const glm::vec3& right) {
        RotationMatrix[0] = glm::vec4(glm::normalize(right), 0.0f);
    }

    void setUpVector(const glm::vec3& up) {
        RotationMatrix[1] = glm::vec4(glm::normalize(up), 0.0f);
    }

    // Static list of all PhysicObject instances
    inline static std::vector<PhysicObject*> allPhysicObjects{}; 

    inline static void deleteObject(PhysicObject* obj){
        auto it = std::find(allPhysicObjects.begin(), allPhysicObjects.end(), obj);
        if (it != allPhysicObjects.end()) {
            allPhysicObjects.erase(it);
        }
    };

    virtual void BeforeCollide(PhysicObject* other, CollisionInfo info);
    virtual void OnCollide(PhysicObject* other, CollisionInfo info);

    static float Length2(const glm::vec3& v);

    static CollisionInfo Box2Box(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Box2Sphere(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Box2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Sphere2Sphere(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Sphere2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Capsule2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo checkCollision(PhysicObject* objA, PhysicObject* objB);

    static void ResolveCollision(PhysicObject* objA, PhysicObject* objB, const CollisionInfo& collisionInfo);
    static std::string ShapeTypeToString(ShapeType type);
};

std::ostream& operator<<(std::ostream& os, const PhysicObject& obj);