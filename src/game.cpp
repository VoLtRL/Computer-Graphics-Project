#include "game.h"
#include "capsule.h"
#include <GLFW/glfw3.h>
#include "handlePhysics.h"

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

    ResourceManager::LoadShader(shaderDir + "world.vert", shaderDir + "world.frag", "world");
    ResourceManager::LoadShader(shaderDir + "player.vert", shaderDir + "player.frag", "player");
    ResourceManager::LoadShader(shaderDir + "crosshair.vert", shaderDir + "crosshair.frag", "crosshair");

    ResourceManager::LoadTexture(imageDir + "crosshair.png", "crosshair");

    Shader* worldShader = ResourceManager::GetShader("world");
    Shader* playerShader = ResourceManager::GetShader("player");

    glm::vec4 fogColor(0.2f, 0.2f, 0.2f, 1.0f);
    float fogStart = 20.0f;
    float fogEnd = 100.0f;
    
    std::vector<GLuint> shaderIds = {worldShader->get_id(), playerShader->get_id()};
    for (GLuint shaderId : shaderIds){
        glUseProgram(shaderId);
        glUniform4fv(glGetUniformLocation(shaderId, "fogColor"), 1, &fogColor[0]);
        glUniform1f(glGetUniformLocation(shaderId, "fogStart"), fogStart);
        glUniform1f(glGetUniformLocation(shaderId, "fogEnd"), fogEnd);  
    }

    glUseProgram(worldShader->get_id());
    glUniform3f(glGetUniformLocation(worldShader->get_id(), "objectColor"), 0.8f, 0.8f, 0.8f);
    glUseProgram(playerShader->get_id());
    glUniform3f(glGetUniformLocation(playerShader->get_id(), "objectColor"), 0.22f, 0.65f, 0.92f);

    // physics
	std::vector<PhysicObject*> mapPhysicObjects;

    //Load map
    Map* map = new Map(worldShader, viewer->scene_root, mapPhysicObjects);
    
    for(auto obj : mapPhysicObjects) {
        handlePhysics->AddObject(obj);
    }

    Shape* playerShape = new Capsule(playerShader, 0.6f, 1.0f);
    player = new Player(playerShape, glm::vec3(0.0f, 2.0f, 0.0f), playerShader);
    player->Mass = 70.0f;
    player->Damping = 0.1f;

    viewer->scene_root->add(player);
    handlePhysics->AddObject(player);

    crosshair = new Crosshair(0.1f);
    crosshairTexture = ResourceManager::GetTexture("crosshair");
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

    if(player->Position.y <= 0.5f){
        player->Position.y = 0.5f;
        player->Velocity.y = 0.0f;
    }

    glm::vec3 camOffset(0.0f, 1.4f, 0.0f);

    viewer->camera->SetTarget(player->Position + camOffset);

    glm::vec3 camFront = viewer->camera->Front;
    camFront.y = 0.0f;
    camFront = glm::normalize(camFront);

    player->FrontVector = camFront;
    
    player->RightVector = glm::normalize(glm::cross(player->FrontVector, glm::vec3(0.0f, 1.0f, 0.0f)));
    player->UpVector    = glm::normalize(glm::cross(player->RightVector, player->FrontVector));
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