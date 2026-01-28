#include "game.h"
#include "capsule.h"
#include <GLFW/glfw3.h>
#include "handlePhysics.h"
#include "sphere.h"
#include "sprite.h"

#include "model.h"
#include "entityLoader.h"
#include "constants.h"
#include "projectile.h"

Game::Game(Viewer* v) : viewer(v) {
    handlePhysics = new HandlePhysics(v->scene_root);
}

Game::~Game() {
    delete handlePhysics;
    delete crosshair;
}

void Game::Init() {
    std::string shaderDir = SHADER_DIR;
    std::string imageDir = IMAGE_DIR;
    std::string fontDir = FONT_DIR;

    if (ResourceManager::GetShader("standard") == nullptr) {
        ResourceManager::LoadShader(shaderDir + "standard.vert", shaderDir + "standard.frag", "standard");
    }
        if (ResourceManager::GetTexture("crosshair") == 0) {
        ResourceManager::LoadTexture(imageDir + "crosshair.png", "crosshair");
    }
    if (ResourceManager::GetShader("sprite") == nullptr) {
        ResourceManager::LoadShader(shaderDir + "sprite.vert", shaderDir + "sprite.frag", "sprite");
    }
    if (ResourceManager::GetShader("text") == nullptr) {
        ResourceManager::LoadShader(shaderDir + "text.vert", shaderDir + "text.frag", "text");
    }

    viewer->scene_root->recursiveReset();

    // load font
    textRenderer = new TextRenderer(Config::SCR_WIDTH, Config::SCR_HEIGHT);
    textRenderer->Load(fontDir + "JetBrains-Mono-Nerd-Font-Complete.ttf", 24);

    Shader* StandardShader = ResourceManager::GetShader("standard");

    spriteRenderer = new Sprite(ResourceManager::GetShader("sprite"));

    // Set fog uniforms
    fogColor = Config::Game::fogColor;
    fogStart = Config::Game::fogStartDistance;
    fogEnd = Config::Game::fogEndDistance;
    // Set sky color
    skyColor = glm::vec3(0.2f, 0.2f, 0.2f);
    viewer->backgroundColor = skyColor;

    enemyKilled = 0;
    hasWon = false;
    isTimeRecorded = false;
    timeRecorded = 0.0;

    glUseProgram(StandardShader->get_id());
    this->fogColorLocation = glGetUniformLocation(StandardShader->get_id(), "fogColor");
    this->fogStartLocation = glGetUniformLocation(StandardShader->get_id(), "fogStart");
    this->fogEndLocation = glGetUniformLocation(StandardShader->get_id(), "fogEnd");

    //Load map
    Map* map = new Map(StandardShader, viewer->scene_root);
    
    if(!player){
            //Load player
            player = EntityLoader::CreatePlayer(glm::vec3(0.0f, 10.0f, 0.0f));
            viewer->scene_root->add(player);
    }

    // load stats menu
    statsMenu = new StatsMenu(textRenderer, player);

    // Mobs Spawners
    EnemySpawner* spawner1 = EntityLoader::CreateEnemySpawner(viewer->scene_root, player->Position, enemies);
    viewer->scene_root->add(spawner1);
	enemySpawner = spawner1;

    // Add a boulder prop
    PhysicShapeObject* boulder = EntityLoader::Boulder(glm::vec3(10.0f, 5.0f, 10.0f), 2.0f, 200.0f);
    viewer->scene_root->add(boulder);

    PhysicShapeObject* boulder2 = EntityLoader::Boulder(glm::vec3(8.0f, 5.0f, 12.0f), 0.5f, 15.0f);
    viewer->scene_root->add(boulder2);


    // crosshair setup
    crosshair = new Crosshair(0.1f);
    crosshairTexture = ResourceManager::GetTexture("crosshair");

    // Load Textures
    gameOverTexture = ResourceManager::LoadTexture(imageDir + "game_over.png", "gameOver");
    healthBarTexture = ResourceManager::LoadTexture(imageDir + "white_dot.png", "healthBar");
    experienceBarTexture = ResourceManager::LoadTexture(imageDir + "white_dot.png", "experienceBar");
    victoryTexture = ResourceManager::LoadTexture(imageDir + "victory_screen.png", "victory");

    Shape* camShape = new Sphere(StandardShader, 0.5f);
    viewer->camera->collisionShape = camShape;
    viewer->camera->collisionGroup = CG_PLAYER;
    viewer->camera->collisionMask = CG_ENVIRONMENT;
    viewer->camera->SetMass(1.0f);

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
        float rayLength = 25.0f;
        glm::vec3 camPos = viewer->camera->Position;
        glm::vec3 camFront = viewer->camera->Front;
        
        glm::vec3 worldTarget = camPos + (camFront * rayLength);

        glm::vec3 gunSpawnPos = player->Position + glm::vec3(0.3f, 0.5f, -0.2f);
        glm::vec3 finalShootDir = glm::normalize(worldTarget - gunSpawnPos);

        player->shoot(finalShootDir);
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

    // display stats menu
    if (viewer->keymap[GLFW_KEY_V]) {
        statsMenu->setVisible(!statsMenu->getIsVisible());
        viewer->keymap[GLFW_KEY_V] = false;
    }
}

void Game::ProcessGameOverInput() {
    GLFWwindow* window = glfwGetCurrentContext();
    if (viewer->keymap[GLFW_KEY_ESCAPE]) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if(viewer->keymap[GLFW_KEY_R]) {
        viewer->keymap[GLFW_KEY_R] = false;
        // delete enemies
        for(auto enemy : enemies){
            viewer->scene_root->remove(enemy);
            delete enemy;
        }
        enemies.clear();

        // delete boulders
        auto& allObjects = PhysicObject::allPhysicObjects;
        for (auto it = allObjects.begin(); it != allObjects.end(); ) {
            PhysicObject* obj = *it;
            // Check if the object is named "Boulder" (case sensitive)
            if (obj->name.find("Boulder") != std::string::npos) {
                if (PhysicShapeObject* pso = dynamic_cast<PhysicShapeObject*>(obj)) {
                    viewer->scene_root->recursiveRemove(pso);
                }
                delete obj;
                // Reset iterator because 'allObjects' is modified by delete
                it = allObjects.begin(); 
            } else {
                ++it;
            }
        }

        // reset spawners
        viewer->scene_root->remove(enemySpawner);
        delete enemySpawner;

        resetGameTime = glfwGetTime();

        player->resetPlayerState(glm::vec3(0.0f, 10.0f, 0.0f));

        this->Init();
    }
}

void Game::Update() {
    if (!player->isAlive() || hasWon) {
        return;
    }
    float deltaTime = viewer->deltaTime;

    // Process inputs
    ProcessInput(deltaTime);
    player->update(deltaTime);

	enemySpawner->Position = player->Position;
	enemySpawner->Update(deltaTime);
    bool isAffraid = (player->temporaryItems.find("Fear") != player->temporaryItems.end());
    auto it = enemies.begin();
    while (it != enemies.end()) {
        Enemy* enemy = *it;
        if (!enemy->isAlive()) {
            enemyKilled++;
            player->addExperience(enemy->getExperienceReward());
            glm::vec3 startColor = glm::vec3(0.2f, 0.2f, 0.2f); // gray
            glm::vec3 endColor   = glm::vec3(0.53f, 0.81f, 0.92f); // blue

            float ratio = (float)enemyKilled / (float)Config::Game::EnemiesToWin;
            ratio = glm::clamp(ratio, 0.0f, 1.0f);
            glm::vec3 newColor = glm::mix(startColor, endColor, ratio);
            
            if(enemyKilled >= Config::Game::EnemiesToWin && !getHasWon()) {
                setHasWon(true);
            }

            this->fogColor = glm::vec4(newColor, 1.0f);
            viewer->backgroundColor = newColor;

            fogStart += 0.5f;
            fogEnd += 1.0f;

			float dropChance = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 100.0f;
			std::map<std::string, double> lootProbabilities = getLootTableProbabilities(enemy->getRarityCoefficient());
			float cumulativeProbability = 0.0f;
			for (const auto& item : lootProbabilities) {
				if ( dropChance <= item.second + cumulativeProbability) {
					if (item.first == "") {
                        break; // No item dropped
                    }
                    Sphere* pshape = new Sphere(ResourceManager::GetShader("standard"), 0.3f);
                    float lifetime = 0.0f;
					if (item.first == "DamageBoost") {
                        pshape->color = glm::vec3(1.0f, 0.0f, 0.0f);
						lifetime = 10.0f;
                    } else if (item.first == "SpeedBoost") {
                        pshape->color = glm::vec3(1.0f, 1.0f, 0.0f);
						lifetime = 10.0f;
                    } else if (item.first == "HealthPack") {
                        pshape->color = glm::vec3(0.0f, 1.0f, 0.0f);
					} else if (item.first == "Fear") {
                        pshape->color = glm::vec3(1.0f, 0.0f, 0.0f);
						lifetime = 5.0f;
                    }

					std::cout << "Dropping item: " << item.first << " | Probability : " << dropChance << std::endl;

                    Pickup* drop = new Pickup(pshape, enemy->Position);
                    drop->name = item.first;
                    drop->collisionShape = pshape;
                    drop->lifetime = lifetime;
                    viewer->scene_root->add(drop);
					break;
                }
                cumulativeProbability += item.second;
            }


            viewer->scene_root->remove(enemy);
            delete enemy;
            it = enemies.erase(it);
        } else {
            enemy->moveTowardsPlayer(player->Position, deltaTime, isAffraid);
            it++;
        }
    }

    glm::vec3 camOffset(0.0f, 2.5f, 0.0f);
    viewer->camera->SetTarget(player->Position + camOffset);

    glm::vec3 camFront = viewer->camera->Front;
    camFront.y = 0.0f;
    camFront = glm::normalize(camFront);

    player->setFrontVector(camFront);
    for(auto enemy : enemies){
        glm::vec3 directionToPlayer = -glm::normalize(player->Position - enemy->Position);
		if (isAffraid) {
            directionToPlayer = -directionToPlayer;
        }
        enemy->setFrontVector(directionToPlayer);
    }
    
    player->setRightVector(glm::normalize(glm::cross(player->GetFrontVector(), glm::vec3(0.0f, 1.0f, 0.0f))));
    player->setUpVector(glm::normalize(glm::cross(player->GetRightVector(), player->GetFrontVector())));

    handlePhysics->Update(deltaTime);

    Shader* standardShader = ResourceManager::GetShader("standard");
    glUseProgram(standardShader->get_id());
    
    // send fog parameters to shader
    glUniform4fv(fogColorLocation, 1, &fogColor[0]);
    glUniform1f(fogStartLocation, fogStart);
    glUniform1f(fogEndLocation, fogEnd);

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

            if (proj->shape) {
                glm::vec3 c = proj->shape->color;
                lightColors.push_back(c.x);
                lightColors.push_back(c.y);
                lightColors.push_back(c.z);
            }

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
}

void Game::RenderDeathUI() {
    if(!isTimeRecorded) {
        isTimeRecorded = true;
        timeRecorded = glfwGetTime() - resetGameTime;
    }
    float aspectRatio = static_cast<float>(Config::SCR_WIDTH) / static_cast<float>(Config::SCR_HEIGHT);
    
    // sprite renderer setup
    Shader* spriteShader = ResourceManager::GetShader("sprite");
    // set the sprite shader
    glUseProgram(spriteShader->get_id());

    glm::mat4 projection = Sprite::getProjection(aspectRatio);
    glUniformMatrix4fv(glGetUniformLocation(spriteShader->get_id(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform1i(glGetUniformLocation(spriteShader->get_id(), "image"), 0);

    // game over screen
    spriteRenderer->draw(gameOverTexture, glm::vec2(0.0f, 0.0f), glm::vec2(1.5f * aspectRatio, 1.5f));

    // Time survived
    int totalSeconds = static_cast<int>(timeRecorded);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    char timeBuffer[30];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "Time Survived : %02d:%02d", minutes, seconds);
    textRenderer->RenderText(timeBuffer, (Config::SCR_WIDTH / 2) - 150.0f, (Config::SCR_HEIGHT / 2) - 150.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Restart
    std::string restartPrompt = "Press 'R' to Restart";
    textRenderer->RenderText(restartPrompt, (Config::SCR_WIDTH / 2) - 140.0f, (Config::SCR_HEIGHT / 2) - 200.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Game::RenderWinUI() {
    if(!isTimeRecorded) {
        isTimeRecorded = true;
        timeRecorded = glfwGetTime() - resetGameTime;
    }
    float aspectRatio = static_cast<float>(Config::SCR_WIDTH) / static_cast<float>(Config::SCR_HEIGHT);
    
    // sprite renderer setup
    Shader* spriteShader = ResourceManager::GetShader("sprite");
    // set the sprite shader
    glUseProgram(spriteShader->get_id());

    glm::mat4 projection = Sprite::getProjection(aspectRatio);
    glUniformMatrix4fv(glGetUniformLocation(spriteShader->get_id(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform1i(glGetUniformLocation(spriteShader->get_id(), "image"), 0);

    // You Win screen
    spriteRenderer->draw(victoryTexture, glm::vec2(0.0f, 0.0f), glm::vec2(1.5f * aspectRatio, 1.5f));

    // display time taken to win
    int totalSeconds = static_cast<int>(timeRecorded);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    char timeBuffer[40];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "Time Taken to Purify: %02d:%02d", minutes, seconds);
    textRenderer->RenderText(timeBuffer, (Config::SCR_WIDTH / 2) - 180.0f, (Config::SCR_HEIGHT / 2) - 150.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Restart
    std::string restartPrompt = "Press 'R' to Restart";
    textRenderer->RenderText(restartPrompt, (Config::SCR_WIDTH / 2) - 140.0f, (Config::SCR_HEIGHT / 2) - 200.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Game::RenderUI() {
    float aspectRatio = static_cast<float>(Config::SCR_WIDTH) / static_cast<float>(Config::SCR_HEIGHT);
    
    // HUD rendering
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // crosshair
    crosshair->draw(crosshairTexture, aspectRatio);

    // render level count left bottom corner
    std::string levelText = "Level : " + std::to_string(player->getLevel());
    textRenderer->RenderText(levelText, 50.0f, 85.0f, 1.0f, glm::vec3(1.0f));

    // render kill count top right corner
    std::string killText = "Kills: " + std::to_string(enemyKilled);
    textRenderer->RenderText(killText, Config::SCR_WIDTH - 500.0f, Config::SCR_HEIGHT - 50.0f, 1.0f, glm::vec3(1.0f));

    // render purification progress right top corner
    float purificationPct = static_cast<float>(enemyKilled) / static_cast<float>(Config::Game::EnemiesToWin);
    purificationPct = glm::clamp(purificationPct, 0.0f, 1.0f);
    int pctDisplay = static_cast<int>(purificationPct * 100.0f);
    std::string pctText = "Purification of the world : " + std::to_string(pctDisplay) + "%";
    textRenderer->RenderText(pctText, Config::SCR_WIDTH - 500.0f, Config::SCR_HEIGHT - 20.0f , 1.0f, glm::vec3(1.0f));

    // render in-game timer center top
    int totalSeconds = static_cast<int>(glfwGetTime() - resetGameTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    char timeBuffer[6];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", minutes, seconds);
    textRenderer->RenderText(timeBuffer, (Config::SCR_WIDTH / 2) - 50.0f, Config::SCR_HEIGHT - 20.0f, 1.0f, glm::vec3(1.0f));

    // render stats menu
    statsMenu->renderMenu();

    // sprite renderer setup
    Shader* spriteShader = ResourceManager::GetShader("sprite");
    // set the sprite shader
    glUseProgram(spriteShader->get_id());

    glm::mat4 projection = Sprite::getProjection(aspectRatio);
    glUniformMatrix4fv(glGetUniformLocation(spriteShader->get_id(), "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniform1i(glGetUniformLocation(spriteShader->get_id(), "image"), 0);
    
    // health bar
    float healthPct = player->getHealth() / player->getMaxHealth();
    healthPct = glm::clamp(healthPct, 0.0f, 1.0f);

    float barWidth = 0.8f;
    float barHeight = 0.05f;
        
    float margin = 0.1f;
    
    float currentWidth = barWidth * healthPct;
    float xPos = -aspectRatio + margin + (currentWidth / 2.0f); 
    float yPos = -0.9f;

    float bgXPos = -aspectRatio + margin + (barWidth / 2.0f);

    // front bar (current health)
    spriteRenderer->draw(healthBarTexture, glm::vec2(xPos, yPos), glm::vec2(currentWidth, barHeight), 0.0f, glm::vec3(0.1f, 0.67f, 0.1f));

    // background bar
    spriteRenderer->draw(healthBarTexture, glm::vec2(bgXPos, yPos), glm::vec2(barWidth, barHeight), 0.0f, glm::vec3(0.8f, 0.1f, 0.1f));

    // experience bar
    float expPct = player->getExperience() / player->getExperienceToNextLevel();
    expPct = glm::clamp(expPct, 0.0f, 1.0f);

    float expBarWidth = 0.8f;
    float expBarHeight = 0.02f;

    float expCurrentWidth = expBarWidth * expPct;
    float expXPos = -aspectRatio + margin + (expCurrentWidth / 2.0f);
    float expYPos = -0.93f;

    float expBgXPos = -aspectRatio + margin + (expBarWidth / 2.0f);

    // front bar (current experience)
    spriteRenderer->draw(experienceBarTexture, glm::vec2(expXPos, expYPos), glm::vec2(expCurrentWidth, expBarHeight), 0.0f, glm::vec3(0.78f, 0.87f, 0.89f));

    // background bar
    spriteRenderer->draw(experienceBarTexture, glm::vec2(expBgXPos, expYPos), glm::vec2(expBarWidth, expBarHeight), 0.0f, glm::vec3(0.2f, 0.2f, 0.2f));

    // Restore state
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

std::map<std::string, double> Game::getLootTableProbabilities(float alpha) {
    std::map<std::string, double> adjustedProbabilities;
    
	double total = 0.0f;
    for (const auto& item : lootTable) {
        double adjustedProb = glm::exp(-1*alpha*item.second);
        adjustedProbabilities[item.first] = adjustedProb;
        total += adjustedProb;
	}

    for (auto& item : adjustedProbabilities) {
        item.second *= 100.0f / total;
    }
	return adjustedProbabilities;
}