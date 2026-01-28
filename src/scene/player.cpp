#include "player.h"
#include <glm/gtc/matrix_transform.hpp>
#include "projectile.h"
#include "sphere.h"
#include "enemy.h"
#include "entityLoader.h"
#include <vector>
#include <GLFW/glfw3.h>
#include <algorithm>

Player::Player(Shape* shape, glm::vec3 position,Shader* projectileShader)
    : PhysicShapeObject(shape, position),
      health(100.0f),
      maxHealth(100.0f),
      movementSpeed(5.0f),
      jumpStrength(10.0f),
      attackDamage(10.0f),
      attackSpeed(2.0f),
      level(1),
      experience(0.0f),
      experienceToNextLevel(100.0f),
      size(1.0f),
      isJumping(false),
      canJump(false),
      attackCooldown(0.0f),
      groundDamping(8.0f),
      projectileSpeed(60.0f),
      projectileShader(projectileShader),
      PreviousPosition(position)

{}

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
    /* std::cout << "Player position: (" 
              << Position.x << ", " 
              << Position.y << ", " 
              << Position.z << ")" << std::endl;
    std::cout << "is Jumping ? " 
              << isJumping << std::endl; */

    // projectile handling
    for(Projectile* proj : activeProjectiles){
        if(proj->isActive()){
            proj->update(deltaTime);
        } else {
            deleteActiveProjectile(proj);
        }
    }

    //level-up check
    if(experience >= experienceToNextLevel){
        levelUp();
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

void Player::shoot(glm::vec3 shootDirection) {
    lastShootTime = glfwGetTime();
    recoilForce = 1.0f;

    if (attackCooldown <= 0.0f) {

        float maxAimDistance = 25.0f; 
        glm::vec3 cameraPos = shootDirection;

        glm::vec3 aimFlat = glm::normalize(glm::vec3(shootDirection.x, 0.0f, shootDirection.z));
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 aimRight = glm::normalize(glm::cross(aimFlat, worldUp)); 

        float rightOffset   = 0.9f;
        float upOffset      = 1.1f;
        float forwardOffset = 0.1f;

        glm::vec3 spawnPos = Position 
                           + (aimRight * rightOffset) 
                           + (worldUp * upOffset) 
                           + (aimFlat * forwardOffset);

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

void Player::levelUp()
{
    addLevel(); //Level up

    setMaxHealth(getMaxHealth()*1.10); //Increase max health by 10%
    setHealth(getHealth()*1.1); //Heal 10% of current health
    setAttackDamage(getAttackDamage()*1.15); //Increase attack damage by 15%
    setSpeed(getSpeed()*(1.0f + std::min(0.5f, getLevel()*0.002f))); //Increase speed by 2% per level, max 50%

    setExperienceToNextLevel(getExperienceToNextLevel() * 1.25f); //Increase XP needed by 25%
    setExperience(experience - experienceToNextLevel); //Carry over excess XP
    
}

void Player::die() {
    if (!isDead) {
        isDead = true;
        /* std::cout << "GAME OVER" << std::endl; */
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

void Player::updateAnimation(float deltaTime) {
    if (!model) return;


    // get current orientation and speed
    glm::vec3 currentFront = glm::normalize(this->GetFrontVector());
    glm::vec3 currentUp    = glm::normalize(this->GetUpVector()); 
    
    glm::vec3 flatVel = Velocity;
    flatVel.y = 0.0f;
    float rawSpeed = glm::length(flatVel);
    bool isMoving = rawSpeed > 0.1f;
    bool isInAir = isJumping || std::abs(Velocity.y) > 0.5f;
    float currentTime = (float)glfwGetTime();

    // Stable reference for strafing
    if (!isMoving) movementReferenceForward = currentFront;
    else movementReferenceForward = glm::normalize(glm::mix(movementReferenceForward, currentFront, deltaTime * 2.0f));
    
    glm::vec3 referenceRight = glm::normalize(glm::cross(currentUp, movementReferenceForward));
    float speedFwd  = glm::dot(flatVel, movementReferenceForward); 
    float speedSide = glm::dot(flatVel, referenceRight);

    // Target initialization
    float targetLeg = 0.0f;
    float targetArmLeft = 0.0f;
    
    // Right arm axes: X=Swing, Y=Recoil, Z=Elevation
    float targetArmRightX = 0.0f; 
    float targetArmRightY = 0.0f; 
    float targetArmRightZ = -0.15f; 
    
    float targetElbowLeft = 0.0f;
    float targetElbowRight = 0.0f;

    float targetLean = 0.0f;   
    float targetTilt = 0.0f;   
    float targetKnee = 0.0f;
    float targetDrop = 0.0f;
    float targetHead = 0.0f;

    // handle landing impact
    if (wasInAir && !isInAir) landingImpact = 1.0f;
    wasInAir = isInAir;
    landingImpact = glm::mix(landingImpact, 0.0f, deltaTime * 5.0f);

    if (isInAir) {
        // Jump pose (scissor arms)
        targetLeg = -0.5f; 
        
        targetArmLeft = -1.2f;      
        targetArmRightX = 0.6f;     
        
        targetElbowLeft = -0.8f;    
        targetElbowRight = 0.8f;   
        
        targetLean = 0.2f;      
        targetKnee = 0.8f; 
        targetHead = -0.2f;
    }
    else if (isMoving) {
        // Procedural walk cycle
        animTime += deltaTime * WALK_FREQ;
        float fwdRatio = glm::clamp(speedFwd / 5.0f, -1.0f, 1.0f);
        float sideRatio = glm::clamp(speedSide / 5.0f, -1.0f, 1.0f);

        float animWalk = glm::sin(animTime) * SWING_AMP * fwdRatio;
        float animStrafe = glm::cos(animTime * 1.5f) * SWING_AMP * 0.5f * sideRatio;

        targetLeg = animWalk + animStrafe;
        
        targetArmLeft = -animWalk * 0.8f; 
        targetArmRightX = animWalk * 0.8f; 
        targetElbowLeft = targetElbowRight = -0.8f; 

        targetLean = -speedFwd * 0.05f; 
        targetLean = glm::clamp(targetLean, -0.3f, 0.3f);
        targetTilt = speedSide * 0.08f; 
        targetTilt = glm::clamp(targetTilt, -0.2f, 0.2f);
        targetKnee = 0.1f;
    }
    else {
        // Idle breathing
        animTime += deltaTime * 2.0f;
        targetArmLeft = targetArmRightX = glm::sin(animTime) * 0.05f;
        targetElbowLeft = targetElbowRight = -0.1f; 
    }

    // Combat layer blending
    recoilForce = glm::mix(recoilForce, 0.0f, deltaTime * 15.0f);

    bool inCombatMode = (currentTime - lastShootTime) < 1.0f;
    float targetBlend = inCombatMode ? 1.0f : 0.0f;
    combatBlend = glm::mix(combatBlend, targetBlend, deltaTime * 8.0f);

    // Shooting pose definition
    float aimArmZ = 1.2f;   
    float aimElbow = 1.8f;  
    float aimArmX = 0.0f;   
    float aimArmY = 0.0f;   

    // Apply recoil to shoulder Y-axis
    aimArmY += - recoilForce * 1.0f; 
    aimArmX += recoilForce * 0.5f;
    aimElbow += recoilForce * 0.3f;

    // Overwrite right arm targets
    targetArmRightX = glm::mix(targetArmRightX, aimArmX, combatBlend);
    targetArmRightY = glm::mix(targetArmRightY, aimArmY, combatBlend); 
    targetArmRightZ = glm::mix(targetArmRightZ, aimArmZ, combatBlend);
    
    targetElbowRight = glm::mix(targetElbowRight, aimElbow, combatBlend);

    // Apply impact effects
    targetDrop = -landingImpact * 0.5f;
    targetLean += landingImpact * 0.5f; 
    targetKnee += landingImpact * 1.5f;
    targetHead += landingImpact * 0.3f;


    // smooth transitions
    float smooth = deltaTime * 10.0f;
    currentLegAngle = glm::mix(currentLegAngle, targetLeg, smooth);
    currentArmAngle = glm::mix(currentArmAngle, targetArmLeft, smooth);
    
    static float currentArmRightX = 0.0f;
    static float currentArmRightY = 0.0f; 
    static float currentArmRightZ = 0.0f;
    
    currentArmRightX = glm::mix(currentArmRightX, targetArmRightX, smooth);
    currentArmRightY = glm::mix(currentArmRightY, targetArmRightY, smooth); 
    currentArmRightZ = glm::mix(currentArmRightZ, targetArmRightZ, smooth);

    static float currentElbowR = 0.0f;
    static float currentElbowL = 0.0f;
    currentElbowR = glm::mix(currentElbowR, targetElbowRight, smooth);
    currentElbowL = glm::mix(currentElbowL, targetElbowLeft, smooth);

    currentLean = glm::mix(currentLean, targetLean, smooth);
    currentTilt = glm::mix(currentTilt, targetTilt, smooth);
    currentKnee = glm::mix(currentKnee, targetKnee, smooth);
    currentDrop = glm::mix(currentDrop, targetDrop, smooth);
    
    static float currentHead = 0.0f;
    currentHead = glm::mix(currentHead, targetHead, smooth);


    // apply transforms to body parts
    if (torso) {
        glm::mat4 t = torsoOrig;
        t = glm::translate(t, glm::vec3(0, currentDrop, 0));
        t = glm::rotate(t, currentLean, glm::vec3(1, 0, 0));
        t = glm::rotate(t, currentTilt, glm::vec3(0, 0, 1));
        torso->set_transform(t);
    } 
    if (head) {
        glm::mat4 t = headOrig;
        t = glm::rotate(t, (-currentLean * 0.8f + currentHead), glm::vec3(1, 0, 0));
        t = glm::rotate(t, combatBlend * -0.5f, glm::vec3(0, 1, 0)); 
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

    if (armLeft) {
        glm::mat4 tL = armLeftOrig;
        float fL = isInAir ? -1.2f : currentArmAngle;
        if(landingImpact > 0.1f) fL = 0.5f;
        tL = glm::rotate(tL, fL, glm::vec3(1,0,0));
        tL = glm::rotate(tL, 0.15f, glm::vec3(0,0,1));
        armLeft->set_transform(tL);
    }

    if (armRight) {
        glm::mat4 tR = armRightOrig;
        float rotX = currentArmRightX;
        if(landingImpact > 0.1f) rotX = 0.5f;
        
        // Apply 3-axis rotation (X, Y, Z)
        tR = glm::rotate(tR, rotX, glm::vec3(1,0,0));
        tR = glm::rotate(tR, currentArmRightY, glm::vec3(0,1,0));
        tR = glm::rotate(tR, currentArmRightZ, glm::vec3(0,0,1));
        
        armRight->set_transform(tR);
    }
    
    if (armLowerLeft && armLowerRight) {
        glm::mat4 tL = armLowerLeftOrig;
        tL = glm::rotate(tL, currentElbowL, glm::vec3(1, 0, 0)); 
        armLowerLeft->set_transform(tL);

        glm::mat4 tR = armLowerRightOrig;
        tR = glm::rotate(tR, currentElbowR, glm::vec3(1, 0, 0));
        armLowerRight->set_transform(tR);
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

void Player::deleteActiveProjectile(Projectile* proj){
    auto it = std::find(activeProjectiles.begin(), activeProjectiles.end(), proj);
    if (it != activeProjectiles.end()) {
        activeProjectiles.erase(it);
    }
    PhysicObject::deleteObject(proj);
}

