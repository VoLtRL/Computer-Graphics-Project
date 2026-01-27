#include "player.h"
#include <glm/gtc/matrix_transform.hpp>
#include "projectile.h"
#include "sphere.h"
#include "enemy.h"
#include "entityLoader.h"
#include <vector>
#include <GLFW/glfw3.h>

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
      canJump(false),
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

void Player::BeforeCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{
    // Placeholder for any pre-collision logic
    if (info.hit) {
        PreviousPosition = Position;
        PreviousVelocity = Velocity;
	}
    Enemy* enemy = dynamic_cast<Enemy*>(other);
    if (enemy && info.hit) {
        enemy->attack(this, deltaTime); // call attack on enemy
    }

	if (glm::dot(this->GetUpVector(), info.normal) > 0.7f) {
		canJump = true;
    }
}

void Player::OnCollide(PhysicObject* other, CollisionInfo info, float deltaTime)
{
    bool isGround = glm::dot(this->GetUpVector(), info.normal) > 0.7f;
    if (isJumping && info.hit && isGround && PhysicObject::Length2(PreviousPosition-Position) < 0.05f) {
        isJumping = false; // Reset jumping state when colliding with ground
    }
    else {
        //std::cout << PhysicObject::Length2(PreviousPosition - Position) << std::endl;
    }
}

void Player::update(float deltaTime)
{
	canJump = false; // reset jump ability each frame
    if (isDead) {
        if (model && deathTimer == 0.0f) {
            glm::mat4 current = model->get_transform();
            model->set_transform(glm::rotate(current, glm::radians(90.0f), glm::vec3(1,0,0)));
        }
        deathTimer += deltaTime;
        return;
    }

    if (health <= 0.0f) {
        die();
        return;
    }

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
        } else {
            deleteActiveProjectile(proj);
        }
    }

    // handle animations
    updateAnimation(deltaTime);
}

void Player::draw(glm::mat4& view, glm::mat4& projection){

    glm::mat4 model = glm::mat4(1.0f);
    
    // Position
    model = glm::translate(model, this->Position);
    //For good positioning on the platform
    model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));

    // Orientation
    glm::mat4 rotation = glm::inverse(glm::lookAt(glm::vec3(0.0f), this->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f)));
    model = model * rotation;

    // Scale
    model = glm::scale(model, glm::vec3(this->size));

    // Draw
    this->model->draw(model, view, projection);

    // Projectiles
    for (auto p : activeProjectiles) {
        p->draw(view, projection);
    }
}

void Player::jump(){
    if(!isJumping && canJump){
        Velocity.y += jumpStrength; // basic jump impulse
        isJumping = true; // set jumping state
    }
}

void Player::gainJumpStrength(float quantity){
    jumpStrength += quantity;
}

void Player::shoot(glm::vec3 shootDirection){

    if(attackCooldown <= 0.0f){
        
        glm::vec3 shootingOrigin = Position + (GetUpVector() * 0.6f) + (GetFrontVector() * 0.5f);
        
        float spawnDistance = 0.5f;
        glm::vec3 spawnPos = shootingOrigin + (shootDirection * spawnDistance);

        Projectile* proj = EntityLoader::CreateProjectile(spawnPos, shootDirection, this);

        activeProjectiles.push_back(proj);
        attackCooldown = 1.0f / attackSpeed;
    }
}

void Player::move(glm::vec3 direction)
{
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

void Player::die() {
    if (!isDead) {
        isDead = true;
        std::cout << "GAME OVER" << std::endl;
    }
}

void Player::resize(float scale)
{
    size *= scale;
    
    if (collisionShape &&collisionShape->shapeType == ShapeType::ST_SPHERE) {
        ((Sphere*)collisionShape)->radius *= scale;
    }
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
void Player::updateAnimation(float deltaTime)
{
    if (!model) return;

    //Player moving check
    bool isMoving = glm::length(glm::vec2(Velocity.x, Velocity.z)) > 0.1f;

    if (isMoving) {

        float speed = 10.0f; 
        float time = glfwGetTime();
        
        float angle = glm::sin(time * speed) * 0.8f; 

        // Rotation + base
        if (armLeft)  armLeft->set_transform(glm::rotate(armLeftOrig, angle, glm::vec3(1, 0, 0)));
        if (armRight) armRight->set_transform(glm::rotate(armRightOrig, -angle, glm::vec3(1, 0, 0)));
        if (legLeft)  legLeft->set_transform(glm::rotate(legLeftOrig, -angle, glm::vec3(1, 0, 0)));
        if (legRight) legRight->set_transform(glm::rotate(legRightOrig, angle, glm::vec3(1, 0, 0)));
    } 
    else {
        // Reset
        if (armLeft)  armLeft->set_transform(armLeftOrig);
        if (armRight) armRight->set_transform(armRightOrig);
        if (legLeft)  legLeft->set_transform(legLeftOrig);
        if (legRight) legRight->set_transform(legRightOrig);
    }
}

// Recursive search for a node by name
Node* recursiveFind(Node* node, std::string name) {
    if (node->name.find(name) != std::string::npos) return node;
    for (auto child : node->children_) { 
        Node* res = recursiveFind(child, name);
        if (res) return res;
    }
    return nullptr;
}

void Player::setModel(Node* modelNode) {
    this->model = modelNode;

    //Find body parts
    armLeft = recursiveFind(model, "Arm_upper_L");
    armRight = recursiveFind(model, "Arm_upper_R");
    legLeft = recursiveFind(model, "Leg_upper_L");
    legRight = recursiveFind(model, "Leg_upper_R");

    //Save initial transforms
    if(armLeft) armLeftOrig = armLeft->get_transform();
    if(armRight) armRightOrig = armRight->get_transform();
    if(legLeft) legLeftOrig = legLeft->get_transform();
    if(legRight) legRightOrig = legRight->get_transform();
}

void Player::deleteActiveProjectile(Projectile* proj){
    auto it = std::find(activeProjectiles.begin(), activeProjectiles.end(), proj);
    if (it != activeProjectiles.end()) {
        activeProjectiles.erase(it);
    }
    PhysicObject::deleteObject(proj);
}

