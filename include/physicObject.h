#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm> // Added for std::find in destructor
#include <cmath>

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

struct CapsuleCollision {
    glm::vec3 A;
    glm::vec3 B;
    float radius;
};

class Shape;

enum class ShapeType {
    ST_BOX,
    ST_SPHERE,
    ST_CAPSULE,
	ST_INVALID
};

std::ostream& operator<<(std::ostream& os, const ShapeType& st);

enum CollisionGroup : uint32_t {
    CG_NONE = 0,
    CG_PLAYER = 1 << 0,
    CG_ENEMY = 1 << 1,
    CG_PLAYER_PROJECTILE = 1 << 2,
    CG_ENEMY_PROJECTILE = 1 << 3,
    CG_ENVIRONMENT = 1 << 4,
    CG_PRESETS_MAP = CG_PLAYER | CG_ENEMY | CG_PLAYER_PROJECTILE | CG_ENEMY_PROJECTILE | CG_ENVIRONMENT,
    CG_PRESETS_PLAYER = CG_ENVIRONMENT | CG_ENEMY | CG_ENEMY_PROJECTILE,
	CG_PRESETS_ENEMY = CG_ENVIRONMENT | CG_PLAYER | CG_PLAYER_PROJECTILE | CG_ENEMY,
};

enum class CollisionResponse {
    CR_NONE,       // ignore
    CR_TRIGGER,    // events only
    CR_PHYSICAL,   // blocage physique
    CR_BOTH        // blocage + events
};

std::ostream& operator<<(std::ostream& os, const CollisionResponse& cr);

class PhysicObject {

public:
    PhysicObject(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    virtual ~PhysicObject(); 

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
	CollisionResponse collisionResponse = CollisionResponse::CR_BOTH;
    glm::vec3 forcesApplied;
    Shape* collisionShape;
    float Restitution;
    ShapeType shapeType;
    uint32_t  collisionGroup = CG_NONE;
    uint32_t  collisionMask = CG_NONE;

    // Update physics state (Integration only)
    virtual void UpdatePhysics(float deltaTime);

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
    virtual void OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime);

    static float Length2(const glm::vec3& v);
    static float ProjectOBB(const OBBCollision& box, const glm::vec3& axis);
    static glm::vec3 ClosestPointAABB(const glm::vec3& p, const glm::vec3& min, const glm::vec3& max);
    static glm::vec3 ClosestPointSegmentAABB(const glm::vec3& A, const glm::vec3& B, const glm::vec3& boxMin, const glm::vec3& boxMax);

    static CollisionInfo Box2Box(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Box2Sphere(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Box2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Sphere2Sphere(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Sphere2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo Capsule2Capsule(PhysicObject* objA, PhysicObject* objB);
    static CollisionInfo checkCollision(PhysicObject* objA, PhysicObject* objB);

    static void ResolveCollision(PhysicObject* objA, PhysicObject* objB, const CollisionInfo& collisionInfo, float deltaTime);
    static std::string ShapeTypeToString(ShapeType type);
};

std::ostream& operator<<(std::ostream& os, const PhysicObject& obj);