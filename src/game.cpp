#include "game.h"
#include "capsule.h"
#include <GLFW/glfw3.h>
#include "handlePhysics.h"
#include "sphere.h"
#include "sprite.h"

#include "model.h"
#include "entityLoader.h"
#include "constants.h"

Game::Game(Viewer* v) : viewer(v) {
    handlePhysics = new HandlePhysics();
}

Game::~Game() {
    delete handlePhysics;
    delete crosshair;
}

void Game::Init() {
    std::string shaderDir = SHADER_DIR;
    std::string imageDir = IMAGE_DIR;

    ResourceManager::LoadShader(shaderDir + "standard.vert", shaderDir + "standard.frag", "standard");
    ResourceManager::LoadTexture(imageDir + "crosshair.png", "crosshair");
    ResourceManager::LoadShader(shaderDir + "sprite.vert", shaderDir + "sprite.frag", "sprite");

    Shader* StandardShader = ResourceManager::GetShader("standard");

    spriteRenderer = new Sprite(ResourceManager::GetShader("sprite"));

    glUseProgram(StandardShader->get_id());
    this->fogColorLocation = glGetUniformLocation(StandardShader->get_id(), "fogColor");
    this->fogStartLocation = glGetUniformLocation(StandardShader->get_id(), "fogStart");
    this->fogEndLocation = glGetUniformLocation(StandardShader->get_id(), "fogEnd");

    //Load map
    Map* map = new Map(StandardShader, viewer->scene_root);
    
    //Load player
    player = EntityLoader::CreatePlayer(glm::vec3(0.0f, 5.0f, 0.0f));
    viewer->scene_root->add(player);


    // Test Cube
    PhysicShapeObject* testBox = EntityLoader::CreateTestBox(glm::vec3(2.0f, 10.0f, 2.0f));
    viewer->scene_root->add(testBox);

    // Mob Spawner
    EnemySpawner* spawner = EntityLoader::CreateEnemySpawner(viewer->scene_root, glm::vec3(0.0f, 1.0f, 0.0f), enemies);
    viewer->scene_root->add(spawner);
    enemySpawners.push_back(spawner);

    crosshair = new Crosshair(0.1f);
    crosshairTexture = ResourceManager::GetTexture("crosshair");

    // Load Textures
    gameOverTexture = ResourceManager::LoadTexture(imageDir + "game_over.png", "gameOver");
    healthBarTexture = ResourceManager::LoadTexture(imageDir + "health_bar.png", "healthBar");
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

    if (viewer->keymap[GLFW_MOUSE_BUTTON_LEFT]){

        float aimDistance = 100.0f;
        glm::vec3 cameraPos = viewer->camera->Position;
        glm::vec3 cameraFront = viewer->camera->Front;
        glm::vec3 aimPoint = cameraPos + (cameraFront * aimDistance);

        glm::vec3 playerGunPos = player->Position + glm::vec3(0.3f, 0.5f, -0.2f);

        glm::vec3 shootDirection = glm::normalize(aimPoint - playerGunPos);

        player->shoot(shootDirection);
    }
    // enable fullscreen toggle
    if (viewer->keymap[GLFW_KEY_F]) {
        static bool isFullscreen = false;
        static int windowedWidth = Config::SCR_WIDTH;
        static int windowedHeight = Config::SCR_HEIGHT;
        static int windowedPosX = 100;
        static int windowedPosY = 100;

        GLFWwindow* window = glfwGetCurrentContext();

        if (!isFullscreen) {
            glfwGetWindowPos(window, &windowedPosX, &windowedPosY);
            glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

            GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

            // Switch to fullscreen
            glfwSetWindowMonitor(window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            // Switch back to windowed mode
            glfwSetWindowMonitor(window, nullptr, windowedPosX, windowedPosY, windowedWidth, windowedHeight, 0);
        }
        isFullscreen = !isFullscreen;

        viewer->keymap[GLFW_KEY_F] = false;
    }
}

void Game::Update() {


    float deltaTime = viewer->deltaTime;

    // send fog parameters to shader
    glUseProgram(ResourceManager::GetShader("standard")->get_id());
    glUniform4fv(fogColorLocation, 1, &fogColor[0]);
    glUniform1f(fogStartLocation, fogStart);
    glUniform1f(fogEndLocation, fogEnd);

    handlePhysics->Update(deltaTime);

    ProcessInput(deltaTime);
    player->update(deltaTime);

    for(auto spawner : enemySpawners){
        spawner->Update(deltaTime);
    }

    auto it = enemies.begin();
while (it != enemies.end()) {
        Enemy* enemy = *it;
        if (!enemy->isAlive()) {
            enemyKilled++;
            glm::vec3 startColor = glm::vec3(0.2f, 0.2f, 0.2f); // gray
            glm::vec3 endColor   = glm::vec3(0.53f, 0.81f, 0.92f); // blue

            float ratio = (float)enemyKilled / (float)Config::Game::EnemiesToWin;
            ratio = glm::clamp(ratio, 0.0f, 1.0f);
            glm::vec3 newColor = glm::mix(startColor, endColor, ratio);

            this->fogColor = glm::vec4(newColor, 1.0f);
            viewer->backgroundColor = newColor;

            fogStart += 0.5f;
            fogEnd += 2.0f;

            viewer->scene_root->remove(enemy);
            delete enemy;
            it = enemies.erase(it);
        } else {
            enemy->moveTowardsPlayer(player->Position, deltaTime);
            it++;
        }
    }

    // Handle Light
    Shader* standardShader = ResourceManager::GetShader("standard");
    glUseProgram(standardShader->get_id());

    int activeCount = 0;
    int MAX_LIGHTS = 100;

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

    glm::vec3 camOffset(0.0f, 2.5f, 0.0f);

    viewer->camera->SetTarget(player->Position + camOffset);

    glm::vec3 camFront = viewer->camera->Front;
    camFront.y = 0.0f;
    camFront = glm::normalize(camFront);

    player->setFrontVector(camFront);
    
    player->setRightVector(glm::normalize(glm::cross(player->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
    player->setUpVector(glm::normalize(glm::cross(player->GetRightVector(), player->GetFrontVector())));
}

void Game::RenderUI() {
    float aspectRatio = static_cast<float>(Config::SCR_WIDTH) / static_cast<float>(Config::SCR_HEIGHT);
    
    // HUD rendering
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // crosshair
    crosshair->draw(crosshairTexture, aspectRatio);

    // sprite renderer setup
    Shader* spriteShader = ResourceManager::GetShader("sprite");
    // set the sprite shader
    glUseProgram(spriteShader->get_id());

    glm::mat4 projection = Sprite::getProjection(aspectRatio);
    glUniformMatrix4fv(glGetUniformLocation(spriteShader->get_id(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform1i(glGetUniformLocation(spriteShader->get_id(), "image"), 0);

    // game over screen
    if (player->getHealth() <= 0.0f) {
        spriteRenderer->draw(gameOverTexture, glm::vec2(0.0f, 0.0f), glm::vec2(1.5f * aspectRatio, 1.5f));
    }
    
    // health bar
    else {
        float healthPct = player->getHealth() / player->getMaxHealth();
        healthPct = glm::clamp(healthPct, 0.0f, 1.0f);
        std::cout << "Health Percentage: " << healthPct << std::endl;
        std::cout << "Current Health: " << player->getHealth() << std::endl;

        float barWidth = 0.8f;
        float barHeight = 0.05f;
        
        float margin = 0.1f;
        float currentWidth = barWidth * healthPct;
        float xPos = -aspectRatio + margin + (currentWidth / 2.0f); 
        float yPos = -0.9f;

        float bgXPos = -aspectRatio + margin + (barWidth / 2.0f);

        spriteRenderer->draw(healthBarTexture, glm::vec2(xPos, yPos), glm::vec2(currentWidth, barHeight), 0.0f, glm::vec3(0.1f, 0.67f, 0.1f));

        spriteRenderer->draw(healthBarTexture, glm::vec2(bgXPos, yPos), glm::vec2(barWidth, barHeight), 0.0f, glm::vec3(0.8f, 0.1f, 0.1f));

    }

    // Restore state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
