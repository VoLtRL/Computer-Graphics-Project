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

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif


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
	std::vector<PhysicObject*> physicObjects;

    //Load map
    Map* map = new Map(worldShader, viewer.scene_root, physicObjects);


    Shape* testCylinder = new Cylinder(playerShader, 2.0f, 0.5f, 50);
    PhysicShapeObject* testPhysicObject = new PhysicShapeObject(testCylinder, glm::vec3(0.0f, 10.0f, -5.0f));
	testPhysicObject->Mass = 1.0f;
    viewer.scene_root->add(testPhysicObject);

	physicObjects.push_back(testPhysicObject);

    viewer.update_callback = [&]() {
        // Met à jour la physique des objets
        float deltaTime = viewer.deltaTime;
        for (PhysicObject* obj : physicObjects) {
            obj->UpdatePhysics(deltaTime);
        }
        if (testPhysicObject->Position.y <= 0.5f) {
            testPhysicObject->Velocity = glm::vec3(0.0f, 5.0f, 0.0f);
		}


    };

    viewer.run(); // Lancer la boucle principale du viewer

}