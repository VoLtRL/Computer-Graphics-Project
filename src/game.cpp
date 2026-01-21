#include "game.h"
#include "capsule.h"
#include <GLFW/glfw3.h>
#include "handlePhysics.h"
#include "sphere.h"

Game::Game(Viewer* v) : viewer(v) {
    handlePhysics = new HandlePhysics();
}

Game::~Game() {
    delete handlePhysics;
    delete player;
    delete map;
    delete crosshair;
}

void Game::Init() {
    std::string shaderDir = SHADER_DIR;
    std::string imageDir = IMAGE_DIR;

    ResourceManager::LoadShader(shaderDir + "standard.vert", shaderDir + "standard.frag", "standard");
    ResourceManager::LoadTexture(imageDir + "crosshair.png", "crosshair");

    Shader* StandardShader = ResourceManager::GetShader("standard");

    glm::vec4 fogColor(0.2f, 0.2f, 0.2f, 1.0f);
    float fogStart = 5.0f;
    float fogEnd = 45.0f;

    glUseProgram(StandardShader->get_id());
    glUniform4fv(glGetUniformLocation(StandardShader->get_id(), "fogColor"), 1, &fogColor[0]);
    glUniform1f(glGetUniformLocation(StandardShader->get_id(), "fogStart"), fogStart);
    glUniform1f(glGetUniformLocation(StandardShader->get_id(), "fogEnd"), fogEnd);

    //Load map
    Map* map = new Map(StandardShader, viewer->scene_root);
    

    // Create Player Shape with a blue color and no checkerboard pattern
    Shape* playerShape = new Capsule(StandardShader, 1.0f, 2.0f);
    playerShape->color = glm::vec3(0.22f, 0.65f, 0.92f);
    playerShape->useCheckerboard = false;
    

    player = new Player(playerShape, glm::vec3(0.0f, 10.0f, 0.0f), StandardShader);
    player->SetMass(70.0f);
    player->Damping = 1.0f;
    player->Friction = 10.0f;
    player->collisionShape = playerShape;
	player->shapeType = CAPSULE;
    player->canCollide = true;
    player->name = "Player";

    viewer->scene_root->add(player);

    crosshair = new Crosshair(0.1f);
    crosshairTexture = ResourceManager::GetTexture("crosshair");

    Box* testBoxShape = new Box(StandardShader, 5.0f, 5.0f, 5.0f);
    testBoxShape->color = glm::vec3(0.8f, 0.3f, 0.3f);
    PhysicShapeObject* testBox = new PhysicShapeObject(testBoxShape, glm::vec3(5.0f, 2.5f, 0.0f));
    testBox->SetMass(0.0f);
    testBox->Damping = 1.0f;
    testBox->Friction = 1.0f;
    testBox->collisionShape = testBoxShape;
    testBox->shapeType = BOX;
    testBox->canCollide = true;
    testBox->name = "TestBox";
    viewer->scene_root->add(testBox);

    Sphere* testSphereShape = new Sphere(StandardShader, 2.5f, 20);
    testSphereShape->color = glm::vec3(0.3f, 0.8f, 0.8f);
    PhysicShapeObject* testShpere = new PhysicShapeObject(testSphereShape, glm::vec3(2.0f, 2.5f, 15.0f));
    testShpere->SetMass(50.0f);
    testShpere->Damping = 1.0f;
    testShpere->Friction = 1.0f;
    testShpere->collisionShape = testSphereShape;
    testShpere->shapeType = SPHERE;
    testShpere->canCollide = true;
    testShpere->name = "TestSphere";
    viewer->scene_root->add(testShpere);

    Capsule* testCapsuleShape = new Capsule(StandardShader, 2.0f, 5.0f);
    testCapsuleShape->color = glm::vec3(0.8f, 0.8f, 0.3f);
    PhysicShapeObject* testCapsule = new PhysicShapeObject(testCapsuleShape, glm::vec3(2.0f, 2.5f, -15.0f));
    testCapsule->SetMass(50.0f);
    testCapsule->Damping = 1.0f;
    testCapsule->Friction = 1.0f;
    testCapsule->collisionShape = testCapsuleShape;
    testCapsule->shapeType = CAPSULE;
    testCapsule->canCollide = true;
    testCapsule->name = "TestCapsule";
    viewer->scene_root->add(testCapsule);
}

void Game::ProcessInput(float deltaTime) {
    // get camera directions
    glm::vec3 camFront = viewer->camera->Front;
    camFront.y = 0.0f;
    camFront = glm::normalize(camFront);

    glm::vec3 camRight = viewer->camera->Right;
    camRight.y = 0.0f;
    camRight = glm::normalize(camRight);

    // get movement direction
    glm::vec3 moveDir = glm::vec3(0.0f);

    // adjust movement based on key inputs
    if (viewer->keymap[GLFW_KEY_W]) moveDir += camFront;
    if (viewer->keymap[GLFW_KEY_S]) moveDir -= camFront;
    if (viewer->keymap[GLFW_KEY_A]) moveDir -= camRight;
    if (viewer->keymap[GLFW_KEY_D]) moveDir += camRight;

    // normalize to prevent faster diagonal movement
    if (glm::length(moveDir) > 0.0f) {
        player->move(glm::normalize(moveDir));
    }

    if (viewer->keymap[GLFW_KEY_SPACE]) player->jump();

    if (viewer->keymap[GLFW_KEY_F]){

        float aimDistance = 100.0f;
        glm::vec3 cameraPos = viewer->camera->Position;
        glm::vec3 cameraFront = viewer->camera->Front;
        glm::vec3 aimPoint = cameraPos + (cameraFront * aimDistance);

        glm::vec3 playerGunPos = player->Position + glm::vec3(0.0f, 0.0f, 0.0f);

        glm::vec3 shootDirection = glm::normalize(aimPoint - playerGunPos);

        player->shoot(shootDirection);}
}

void Game::Update() {
    float deltaTime = viewer->deltaTime;

    handlePhysics->Update(deltaTime);

    ProcessInput(deltaTime);
    player->update(deltaTime);

    // Handle Light

    Shader* standardShader = ResourceManager::GetShader("standard");
    glUseProgram(standardShader->get_id());

    int activeCount = 0;
    int MAX_LIGHTS = 32;

    std::vector<float> lightPos;
    std::vector<float> lightColors;
    std::vector<float> lightIntensities;

    
    for (auto proj : player->getActiveProjectiles()) {
        if (proj->isActive() && activeCount < MAX_LIGHTS) {
            lightPos.push_back(proj->Position.x);
            lightPos.push_back(proj->Position.y);
            lightPos.push_back(proj->Position.z);

            lightColors.push_back(1.0f);
            lightColors.push_back(0.8f);
            lightColors.push_back(0.6f);

            lightIntensities.push_back(0.6f);

            activeCount++;
        }
    }

    glUniform1i(glGetUniformLocation(standardShader->get_id(), "numActiveLights"), activeCount);
    
    if (activeCount > 0) {
        glUniform3fv(glGetUniformLocation(standardShader->get_id(), "lightPos"), activeCount, lightPos.data());
        glUniform3fv(glGetUniformLocation(standardShader->get_id(), "lightColors"), activeCount, lightColors.data());
        glUniform1fv(glGetUniformLocation(standardShader->get_id(), "lightIntensities"), activeCount, lightIntensities.data());
    }

    glm::vec3 camOffset(0.0f, 1.4f, 0.0f);

    viewer->camera->SetTarget(player->Position + camOffset);

    glm::vec3 camFront = viewer->camera->Front;
    camFront.y = 0.0f;
    camFront = glm::normalize(camFront);

    player->setFrontVector(camFront);
    
    player->setRightVector(glm::normalize(glm::cross(player->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
    player->setUpVector(glm::normalize(glm::cross(player->GetRightVector(), player->GetFrontVector())));
}

void Game::RenderUI() {
    float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
        glDisable(GL_DEPTH_TEST); // Turn off depth test for UI
        glEnable(GL_BLEND);       // Enable blending for transparency
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        crosshair->draw(crosshairTexture, aspectRatio);

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
}