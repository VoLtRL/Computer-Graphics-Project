#include "entityLoader.h"
#include "resourceManager.h"
#include "box.h"
#include "sphere.h"
#include "capsule.h"
#include "projectile.h"
#include "player.h"

Player* EntityLoader::CreatePlayer(glm::vec3 position){
    Shader* StandardShader = ResourceManager::GetShader("standard");
    std::string imageDir = IMAGE_DIR;

    Shape* playerShape = new Sphere(StandardShader, 0.5f, 20);
    playerShape->color = glm::vec3(0.22f, 0.65f, 0.92f);
    playerShape->useCheckerboard = false;
    
    Player* player = new Player(playerShape, position, StandardShader);
    player->SetMass(70.0f);
    player->Damping = 0.8f;
    player->Friction = 1.0f;
    player->collisionShape = playerShape;
	player->shapeType = ShapeType::ST_CAPSULE;
	player->collisionGroup = CG_PLAYER;
	player->collisionMask = CG_PRESETS_PLAYER;
    player->name = "Player";

    Model* knight = ResourceManager::GetModel("knight");
    if(knight == nullptr){
        knight = ResourceManager::LoadModel( imageDir + "knight.glb", "knight", StandardShader);
    }

    if (knight->rootNode) {
        Node* playerModelNode = knight->rootNode->clone();

        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
        playerModelNode->set_transform(scaleMatrix);

        player->setModel(playerModelNode);
}

    return player;
}

Projectile* EntityLoader::CreateProjectile(glm::vec3 pos, glm::vec3 dir, Player* shooter){
    Shader* StandardShader = ResourceManager::GetShader("standard");
    glm::vec3 shootDirection = glm::normalize(dir);

    Shape* proj_shape = new Sphere(StandardShader, shooter->getSize() * 0.2f, 20);

    proj_shape->color = glm::vec3(1.0f, 0.96f, 0.86f);
    proj_shape->isEmissive = true;

    Projectile* proj = new Projectile(proj_shape, pos, shooter->getProjectileSpeed(), shooter->getAttackDamage(), 100.0f);
    proj->Velocity = shootDirection * shooter->getProjectileSpeed();
    proj->SetMass(0.2f);
    proj->kinematic = false;
    proj->collisionShape = proj_shape;
    proj->shapeType = ShapeType::ST_SPHERE;
    proj->collisionGroup = CollisionGroup::CG_PLAYER_PROJECTILE;
    proj->collisionGroup = CG_PLAYER_PROJECTILE;
	proj->collisionMask = CG_ENEMY | CG_ENVIRONMENT;
    proj->Restitution = 1.0f;

    proj->setFrontVector(shootDirection);
    proj->setRightVector(glm::normalize(glm::cross(proj->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
    proj->setUpVector(glm::normalize(glm::cross(proj->GetRightVector(), proj->GetFrontVector())));

    return proj;
}

Enemy* EntityLoader::CreateEnemy(glm::vec3 position){
    Shader* StandardShader = ResourceManager::GetShader("standard");
    std::string imageDir = IMAGE_DIR;

    Shape* enemyShape = new Capsule(StandardShader, 0.5f, 2.0f);
    enemyShape->color = glm::vec3(0.9f, 0.1f, 0.1f);
    enemyShape->useCheckerboard = false;

    Enemy *enemy = new Enemy(enemyShape, glm::vec3(-5.0f, 10.0f, -5.0f), StandardShader);
    enemy->SetMass(50.0f);
    enemy->Damping = 1.0f;
    enemy->Friction = 1.0f;
    enemy->collisionShape = enemyShape;
    enemy->shapeType = ShapeType::ST_CAPSULE;
    enemy->collisionGroup = CollisionGroup::CG_ENEMY;
    enemy->collisionMask = CollisionGroup::CG_PRESETS_ENEMY;
    enemy->name = "Enemy1";
    enemy->setSpeed(2.0f);
    enemy->setAttackSpeed(1.0f);
    enemy->setPower(10);

    Model* ghostT1 = ResourceManager::GetModel("ghostT1");
    if(ghostT1 == nullptr){
        ghostT1 = ResourceManager::LoadModel( imageDir + "Mob_T1.glb", "ghostT1", StandardShader);
    }

    if (ghostT1->rootNode) {
        Node* ghostT1ModelNode = ghostT1->rootNode->clone();

        ghostT1ModelNode->setAlpha(0.5f);

        enemy->setModel(ghostT1ModelNode);
}

    return enemy;
}

PhysicShapeObject* EntityLoader::CreateTestBox(glm::vec3 position){
    Shader* StandardShader = ResourceManager::GetShader("standard");

    Box* testBoxShape = new Box(StandardShader, 5.0f, 5.0f, 5.0f);
    testBoxShape->color = glm::vec3(0.8f, 0.3f, 0.3f);
    PhysicShapeObject* testBox = new PhysicShapeObject(testBoxShape, glm::vec3(2.0f, 5.0f, 0.0f));
    testBox->SetMass(50.0f);
    testBox->Damping = 1.0f;
    testBox->Friction = 1.0f;
    testBox->collisionShape = testBoxShape;
    testBox->shapeType = ShapeType::ST_BOX;
    testBox->collisionGroup = CollisionGroup::CG_PRESETS_MAP;
    testBox->collisionMask = CollisionGroup::CG_PRESETS_MAP;
    testBox->name = "TestBox";

    return testBox;
}