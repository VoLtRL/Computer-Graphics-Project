#include "entityLoader.h"
#include "resourceManager.h"
#include "box.h"
#include "sphere.h"
#include "capsule.h"
#include "projectile.h"
#include "player.h"
#include "constants.h"

Player* EntityLoader::CreatePlayer(glm::vec3 position){
    Shader* StandardShader = ResourceManager::GetShader("standard");
    std::string imageDir = IMAGE_DIR;

    Shape* playerShape = new Capsule(StandardShader, Config::Player::capsuleRadius , Config::Player::capsuleHeight);
    
    Player* player = new Player(playerShape, position, StandardShader);
    player->SetMass(70.0f);
    player->Damping = 0.8f;
    player->Friction = 1.0f;
    player->collisionShape = playerShape;
	player->collisionGroup = CG_PLAYER;
	player->collisionMask = CG_PRESETS_PLAYER;
    player->name = "Player";

    Model* knight = ResourceManager::GetModel("knight");
    if(knight == nullptr){
        knight = ResourceManager::LoadModel( imageDir + "knight.glb", "knight", StandardShader);
    }

    if (knight->rootNode) {
        Node* playerModelNode = knight->rootNode->clone();

        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(Config::Player::scaleModel));
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
    proj->collisionGroup = CollisionGroup::CG_PLAYER_PROJECTILE;
    proj->collisionGroup = CG_PLAYER_PROJECTILE;
	proj->collisionMask = CG_ENEMY | CG_ENVIRONMENT;
    proj->Restitution = 0.5f;

    proj->setFrontVector(shootDirection);
    proj->setRightVector(glm::normalize(glm::cross(proj->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
    proj->setUpVector(glm::normalize(glm::cross(proj->GetRightVector(), proj->GetFrontVector())));

    return proj;
}

Enemy* EntityLoader::CreateEnemy(glm::vec3 position,int tier){
    Shader* StandardShader = ResourceManager::GetShader("standard");
    std::string imageDir = IMAGE_DIR;

    Shape* enemyShape = new Capsule(StandardShader, Config::Enemy::RADIUS, Config::Enemy::HEIGHT);

    Enemy *enemy = new Enemy(enemyShape, position, StandardShader);
    enemy->SetMass(50.0f);
    enemy->Damping = 3.0f;
    enemy->Friction = 1.0f;
    enemy->collisionShape = enemyShape;
    enemy->collisionGroup = CollisionGroup::CG_ENEMY;
    enemy->collisionMask = CollisionGroup::CG_PRESETS_ENEMY;
    enemy->name = "Enemy1";
    switch (tier)
    {
    case 1:
        {
        enemy->setSpeed(2.0f);
        enemy->setAttackSpeed(1.0f);
        enemy->setPower(10);
        enemy->setTier(1);
        enemy->setHealth(20);
        enemy->setExperienceReward(10.0f);

        Model* ghostT1 = ResourceManager::GetModel("ghostT1");

        if(ghostT1 == nullptr){
            ghostT1 = ResourceManager::LoadModel( imageDir + "Mob_T1.glb", "ghostT1", StandardShader);
        }

        if (ghostT1->rootNode) {
            Node* ghostT1ModelNode = ghostT1->rootNode->clone();

            ghostT1ModelNode->setAlpha(0.2f);

            enemy->setModel(ghostT1ModelNode);
        }

        break;
    }
    case 2:
        {
        enemy->setSpeed(3.0f);
        enemy->setAttackSpeed(1.5f);
        enemy->setPower(20);
        enemy->setTier(2);
        enemy->setHealth(45);
        enemy->setExperienceReward(25.0f);

        Model* ghostT2 = ResourceManager::GetModel("ghostT2");
        if(ghostT2 == nullptr){
            ghostT2 = ResourceManager::LoadModel( imageDir + "Mob_T2.glb", "ghostT2", StandardShader);
        }
        if (ghostT2->rootNode) {
            Node* ghostT2ModelNode = ghostT2->rootNode->clone();

            ghostT2ModelNode->setAlpha(0.2f);

            enemy->setModel(ghostT2ModelNode);
        }

        break;
    }
    case 3:
        {
        enemy->setSpeed(3.75f);
        enemy->setAttackSpeed(2.0f);
        enemy->setPower(30);
        enemy->setTier(3);
        enemy->setHealth(120);
        enemy->setExperienceReward(75.0f);

        Model* ghostT3 = ResourceManager::GetModel("ghostT3");
        if(ghostT3 == nullptr){
            ghostT3 = ResourceManager::LoadModel( imageDir + "Mob_T3.glb", "ghostT3", StandardShader);
        }
        if (ghostT3->rootNode) {
            Node* ghostT3ModelNode = ghostT3->rootNode->clone();
            ghostT3ModelNode->setAlpha(0.2f);
            enemy->setModel(ghostT3ModelNode);
        }

        break;
    }
    case 4:
        {
        enemy->setSpeed(4.25f);
        enemy->setAttackSpeed(2.5f);
        enemy->setPower(40);
        enemy->setTier(4);
        enemy->setHealth(400);
        enemy->setExperienceReward(300.0f);

        Model* ghostT4 = ResourceManager::GetModel("ghostT4");
        if(ghostT4 == nullptr){
            ghostT4 = ResourceManager::LoadModel( imageDir + "Mob_T4.glb", "ghostT4", StandardShader);
        }
        if (ghostT4->rootNode) {
            Node* ghostT4ModelNode = ghostT4->rootNode->clone();
            ghostT4ModelNode->setAlpha(0.2f);
            enemy->setModel(ghostT4ModelNode);
        }

        break;
    }
    default:
        enemy->setSpeed(2.0f);
        enemy->setAttackSpeed(1.0f);
        enemy->setPower(10);
        break;
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
    testBox->collisionGroup = CollisionGroup::CG_PROP;
    testBox->collisionMask = CollisionGroup::CG_PRESETS_PROP;
    testBox->name = "TestBox";

    return testBox;
}

EnemySpawner* EntityLoader::CreateEnemySpawner(Node* sceneRoot, glm::vec3 position, std::vector<Enemy*>& enemyList){
    EnemySpawner* spawner = new EnemySpawner(sceneRoot, position, enemyList);
    spawner->name = "EnemySpawner";
    spawner->collisionGroup = CG_NONE;
    spawner->collisionMask = CG_NONE;
    spawner->SetMass(0.0f); // Immovable
    spawner->kinematic = true;
    return spawner;
}