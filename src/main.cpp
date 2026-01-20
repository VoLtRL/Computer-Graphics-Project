#include "viewer.h"
#include "triangle.h"
#include "cylinder.h"
#include "node.h"
#include "shader.h"
#include <string>
#include <GLFW/glfw3.h> // Nécessaire pour glfwGetTime
#include <camera.h>
#include <physicShapeObject.h>
#include "map.h"
#include <vector>
#include "player.h"

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif

void handle_input(Viewer& viewer, Player& player, float deltaTime) {
        if (viewer.keymap[GLFW_KEY_W]) {
            player.move(glm::vec3(0.0f, 0.0f, -1.0f));
        }
        if (viewer.keymap[GLFW_KEY_S]) {
            player.move(glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (viewer.keymap[GLFW_KEY_A]) {
            player.move(glm::vec3(-1.0f, 0.0f, 0.0f));
        }
        if (viewer.keymap[GLFW_KEY_D]) {
            player.move(glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (viewer.keymap[GLFW_KEY_W] && viewer.keymap[GLFW_KEY_A]) {
            player.move(glm::vec3(-sqrt(2), 0.0f, -sqrt(2)));
        }
        if (viewer.keymap[GLFW_KEY_W] && viewer.keymap[GLFW_KEY_D]) {
            player.move(glm::vec3(sqrt(2), 0.0f, -sqrt(2)));
        }
        if (viewer.keymap[GLFW_KEY_S] && viewer.keymap[GLFW_KEY_A]) {
            player.move(glm::vec3(-sqrt(2), 0.0f, sqrt(2)));
        }
        if (viewer.keymap[GLFW_KEY_S] && viewer.keymap[GLFW_KEY_D]) {
            player.move(glm::vec3(sqrt(2), 0.0f, sqrt(2)));
        }
        if (viewer.keymap[GLFW_KEY_SPACE]) {
            player.jump();
        }
        if (viewer.keymap[GLFW_KEY_K]) {
            player.gainJumpStrength(0.25f);
        }
        if (viewer.keymap[GLFW_KEY_F]) {
            player.shoot();
        }

    }

int main()
{
    Viewer viewer(SCR_WIDTH, SCR_HEIGHT);
    std::string shader_dir = SHADER_DIR;

    //fog parameters
    glm::vec4 fogColor(0.2f, 0.2f, 0.2f, 1.0f);
    float fogStart = 20.0f;
    float fogEnd = 100.0f;

    Shader *worldShader = new Shader(shader_dir + "world.vert", shader_dir + "world.frag");
    Shader *playerShader = new Shader(shader_dir + "player.vert", shader_dir + "player.frag");
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

    //Load map
    Map* map = new Map(worldShader, viewer.scene_root, PhysicObject::allPhysicObjects);

    Shape* playerCube = new Box(playerShader, 1.0f, 1.0f, 1.0f);

    Player* player = new Player(playerCube, glm::vec3(0.0f,20.0f,0.0f),playerShader);
    player->SetMass(70.0f); // mass in kg
    player->Damping = 1.0f; // some damping
	player->collisionShape = playerCube;
	player->shapeType = BOX;
	player->name = "Player";
	PhysicObject::allPhysicObjects.push_back(player);
    viewer.scene_root->add(player);


	Shape* testCube1 = new Box(worldShader, 5.0f, 5.0f, 5.0f);
	PhysicShapeObject* cube1 = new PhysicShapeObject(testCube1, glm::vec3(0.0f, 2.0f, 0.0f));
	cube1->RotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(45.2f), glm::vec3(1.0f, 0.0f, 0.0f));
	cube1->SetMass(0.0f); // immovable
    cube1->collisionShape = testCube1;
	cube1->shapeType = BOX;
    cube1->name = "AnchoredCube";
	cube1->canCollide = true;

    Shape* testCube2 = new Box(worldShader, 3.0f, 3.0f, 3.0f);
	PhysicShapeObject* cube2 = new PhysicShapeObject(testCube2, glm::vec3(7.0f, 10.0f, 0.0f));
	cube2->SetMass(2.0f); // mass in kg
    cube2->Damping = 0.1f; // some damping
	cube2->collisionShape = testCube2;
    cube2->shapeType = BOX;
	cube2->name = "FallingCube";
	cube2->canCollide = true;

    PhysicObject::allPhysicObjects.push_back(cube1);
    PhysicObject::allPhysicObjects.push_back(cube2);

	viewer.scene_root->add(cube1);
	viewer.scene_root->add(cube2);


    viewer.update_callback = [&]() {
        // update physics for all objects
        float deltaTime = viewer.deltaTime;
        for (PhysicObject* obj : PhysicObject::allPhysicObjects) {
            obj->UpdatePhysics(deltaTime);
        }
        // handle player input
        handle_input(viewer,*player,deltaTime);
        player->update(deltaTime);

        // update camera target to follow player
        viewer.camera->SetTarget(player->Position);

    };




    viewer.run(); // Lancer la boucle principale du viewer

}

