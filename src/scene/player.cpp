#include "player.h"
#include <glm/gtc/matrix_transform.hpp>

Player::Player(Shape* shape, glm::vec3 position)
    : PhysicShapeObject(shape, position),
      health(100.0f),
      maxHealth(100.0f),
      movementSpeed(5.0f),
      jumpStrength(10.0f),
      attackDamage(20.0f),
      attackSpeed(1.0f),
      size(1.0f),
      isJumping(false),
      attackCooldown(0.0f),
      groundDamping(8.0f),
      projectileSpeed(25.0f)
{
    std::cout << "Player created at position: (" 
              << position.x << ", " 
              << position.y << ", " 
              << position.z << ")" << std::endl;
}

void Player::update(float deltaTime)
{
    // handle physics
    UpdatePhysics(deltaTime);

    // handle attack cooldown
    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
        if (attackCooldown < 0.0f) {
            attackCooldown = 0.0f;
        }
    }
    // handle ground damping
    if (Position.y <= 0.0f) {
        Velocity.x -= Velocity.x * glm::exp(-groundDamping * deltaTime);
        Velocity.z -= Velocity.z * glm::exp(-groundDamping * deltaTime);
        isJumping = false;
    }   

    std::cout << "Player position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl;
    std::cout << "is Jumping ? " 
              << isJumping << std::endl;

    // handle animations
    updateAnimation(deltaTime);

    // handle orientation
    updateOrientation();
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

void Player::shoot(){
    if(attackCooldown <= 0.0f){
        // Create and launch projectile
        // Projectile* proj = new Projectile(...);
        // proj->Velocity = FrontVector * projectileSpeed;
        attackCooldown = 1.0f / attackSpeed; // Reset cooldown
    }else{
        std::cout << "Attack on cooldown: " << attackCooldown << " seconds remaining." << std::endl;
    }
}

void Player::move(glm::vec3 direction)
{
    if(!isJumping){
        glm::vec3 normDir = glm::normalize(direction);
        Velocity.x = normDir.x * movementSpeed;
        Velocity.z = normDir.z * movementSpeed;
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
void Player::setModel(Node* modelNode) {
    this->model = modelNode;

    // Torso, Head & Limbs
    torso = recursiveFind(model, "Torso");
    head = recursiveFind(model, "Helmet");
    armLeft = recursiveFind(model, "Arm_upper_L");
    armRight = recursiveFind(model, "Arm_upper_R");
    legLeft = recursiveFind(model, "Leg_upper_L");
    legRight = recursiveFind(model, "Leg_upper_R");
    armLowerLeft = recursiveFind(model, "Arm_lower_L");
    armLowerRight = recursiveFind(model, "Arm_lower_R");
    legLowerLeft = recursiveFind(model, "Leg_lower_L");
    legLowerRight = recursiveFind(model, "Leg_lower_R");

    // Store Originals
    if(torso) torsoOrig = torso->get_transform();
    if(head) headOrig = head->get_transform();
    if(armLeft) armLeftOrig = armLeft->get_transform();
    if(armRight) armRightOrig = armRight->get_transform();
    if(legLeft) legLeftOrig = legLeft->get_transform();
    if(legRight) legRightOrig = legRight->get_transform();
    if(armLowerLeft) armLowerLeftOrig = armLowerLeft->get_transform();
    if(armLowerRight) armLowerRightOrig = armLowerRight->get_transform();
    if(legLowerLeft) legLowerLeftOrig = legLowerLeft->get_transform();
    if(legLowerRight) legLowerRightOrig = legLowerRight->get_transform();
}

// Main Animation Loop
void Player::updateAnimation(float deltaTime) {
    if (!model) return;

    // 1. Physics Data
    glm::vec3 currentFront = glm::normalize(this->GetFrontVector());
    glm::vec3 currentUp    = glm::normalize(this->GetUpVector()); 
    
    glm::vec3 flatVel = Velocity;
    flatVel.y = 0.0f;
    float rawSpeed = glm::length(flatVel);
    bool isMoving = rawSpeed > 0.1f;
    bool isInAir = isJumping || std::abs(Velocity.y) > 0.5f;

    // 2. Reference System (Latch Logic)
    if (!isMoving) {
        // Reset reference when stopped
        movementReferenceForward = currentFront;
    }
    else {
        // Slowly rotate reference to allow strafe detection
        movementReferenceForward = glm::mix(movementReferenceForward, currentFront, deltaTime * 2.0f);
        movementReferenceForward = glm::normalize(movementReferenceForward);
    }
    
    glm::vec3 referenceRight = glm::normalize(glm::cross(currentUp, movementReferenceForward));

    // 3. Calculate Relative Speeds
    float speedFwd  = glm::dot(flatVel, movementReferenceForward); 
    float speedSide = glm::dot(flatVel, referenceRight);

    // 4. Calculate Targets
    float targetLeg = 0.0f;
    float targetArm = 0.0f;
    float targetLean = 0.0f;   
    float targetTilt = 0.0f;   
    float targetElbow = 0.0f;  
    float targetKnee = 0.0f;
    float targetDrop = 0.0f;
    float targetHead = 0.0f;

    // Handle Landing Impact
    if (wasInAir && !isInAir) landingImpact = 1.0f;
    wasInAir = isInAir;
    landingImpact = glm::mix(landingImpact, 0.0f, deltaTime * 5.0f);

    if (isInAir) {
        // Jump pose
        targetLeg = -0.5f; 
        targetArm = -1.2f; 
        targetElbow = -1.2f;    
        targetLean = 0.2f;      
        targetKnee = 0.8f; 
        targetHead = -0.2f;
    }
    else if (isMoving) {
        animTime += deltaTime * WALK_FREQ;

        // Normalize ratios
        float fwdRatio = glm::clamp(speedFwd / 5.0f, -1.0f, 1.0f);
        float sideRatio = glm::clamp(speedSide / 5.0f, -1.0f, 1.0f);

        // Mix Walk & Strafe
        float animWalk = glm::sin(animTime) * SWING_AMP * fwdRatio;
        float animStrafe = glm::cos(animTime * 1.5f) * SWING_AMP * 0.5f * sideRatio;

        targetLeg = animWalk + animStrafe;
        targetArm = -animWalk * 0.8f; 
        targetElbow = -0.8f; 

        // Physics Tilt/Lean
        targetLean = -speedFwd * 0.05f; 
        targetLean = glm::clamp(targetLean, -0.3f, 0.3f);
        
        targetTilt = speedSide * 0.08f; 
        targetTilt = glm::clamp(targetTilt, -0.2f, 0.2f);
        
        targetKnee = 0.1f;
    }
    else {
        // Idle
        animTime += deltaTime * 2.0f;
        targetArm = glm::sin(animTime) * 0.05f;
        targetElbow = -0.1f; 
    }

    // Apply Impact
    targetDrop = -landingImpact * 0.5f;
    targetLean += landingImpact * 0.5f; 
    targetKnee += landingImpact * 1.5f;
    targetHead += landingImpact * 0.3f;

    // 5. Smoothing (Lerp)
    float smooth = deltaTime * 10.0f;
    currentLegAngle = glm::mix(currentLegAngle, targetLeg, smooth);
    currentArmAngle = glm::mix(currentArmAngle, targetArm, smooth);
    currentLean     = glm::mix(currentLean, targetLean, smooth);
    currentTilt     = glm::mix(currentTilt, targetTilt, smooth);
    currentElbow    = glm::mix(currentElbow, targetElbow, smooth);
    currentKnee     = glm::mix(currentKnee, targetKnee, smooth);
    currentDrop     = glm::mix(currentDrop, targetDrop, smooth);
    
    static float currentHead = 0.0f;
    currentHead = glm::mix(currentHead, targetHead, smooth);

    // 6. Apply Transforms
    if (torso) {
        glm::mat4 t = torsoOrig;
        t = glm::translate(t, glm::vec3(0, currentDrop, 0));
        t = glm::rotate(t, currentLean, glm::vec3(1, 0, 0));
        t = glm::rotate(t, currentTilt, glm::vec3(0, 0, 1));
        torso->set_transform(t);
    } 

    if (head) {
        glm::mat4 t = headOrig;
        t = glm::rotate(t, -currentLean * 0.8f + currentHead, glm::vec3(1, 0, 0));
        head->set_transform(t);
    }

    if (legLeft && legRight) {
        glm::mat4 tL = legLeftOrig;
        if(!torso) tL = glm::rotate(tL, currentLean, glm::vec3(1,0,0));
        tL = glm::rotate(tL, currentLegAngle - (currentKnee*0.4f), glm::vec3(1, 0, 0));
        legLeft->set_transform(tL);

        glm::mat4 tR = legRightOrig;
        if(!torso) tR = glm::rotate(tR, currentLean, glm::vec3(1,0,0));
        tR = glm::rotate(tR, -currentLegAngle - (currentKnee*0.4f), glm::vec3(1, 0, 0));
        legRight->set_transform(tR);
    }

    if (legLowerLeft && legLowerRight) {
        glm::mat4 tL = legLowerLeftOrig;
        tL = glm::rotate(tL, currentKnee, glm::vec3(1, 0, 0));
        legLowerLeft->set_transform(tL);

        glm::mat4 tR = legLowerRightOrig;
        tR = glm::rotate(tR, currentKnee, glm::vec3(1, 0, 0));
        legLowerRight->set_transform(tR);
    }

    if (armLeft && armRight) {
        glm::mat4 tL = armLeftOrig;
        float fL = isInAir ? currentArmAngle : currentArmAngle;
        if(landingImpact > 0.1f) fL = 0.5f;
        tL = glm::rotate(tL, fL, glm::vec3(1,0,0));
        tL = glm::rotate(tL, 0.15f, glm::vec3(0,0,1));
        armLeft->set_transform(tL);

        glm::mat4 tR = armRightOrig;
        float fR = isInAir ? -currentArmAngle*0.5f : -currentArmAngle;
        if(landingImpact > 0.1f) fR = 0.5f;
        tR = glm::rotate(tR, fR, glm::vec3(1,0,0));
        tR = glm::rotate(tR, -0.15f, glm::vec3(0,0,1));
        armRight->set_transform(tR);
    }
    
    if (armLowerLeft && armLowerRight) {
        glm::mat4 tL = armLowerLeftOrig;
        tL = glm::rotate(tL, currentElbow, glm::vec3(1, 0, 0)); 
        armLowerLeft->set_transform(tL);

        glm::mat4 tR = armLowerRightOrig;
        float rE = isInAir ? currentElbow * 0.3f : currentElbow; 
        tR = glm::rotate(tR, rE, glm::vec3(1, 0, 0));
        armLowerRight->set_transform(tR);
    }
}


void Player::updateOrientation()
{
    // TODO
}

