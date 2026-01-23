#include "viewer.h"
#include "camera.h"

#include <iostream>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "resourceManager.h"
#include "shader.h"
#include "constants.h"


Viewer::Viewer(int width, int height)
{
    if (!glfwInit())    // initialize window system glfw
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        glfwTerminate();
    }

    // version hints: create GL window with >= OpenGL 3.3 and core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    win = glfwCreateWindow(width, height, "Viewer", NULL, NULL);

    if (win == NULL) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
    }

    // make win's OpenGL context current; no OpenGL calls can happen before
    glfwMakeContextCurrent(win);

    // Initialisation de GLAD 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return;
    }

    // Set user pointer for GLFW window to this Viewer instance
    glfwSetWindowUserPointer(win, this);

    // register event handlers
    glfwSetKeyCallback(win, key_callback);

    // Mouse movement callback
    glfwSetCursorPosCallback(win, mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // useful message to check OpenGL renderer characteristics
    std::cout << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << ", Renderer "
              << glGetString(GL_RENDERER) << std::endl;

    // initialize GL by setting viewport and default render characteristics
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    /* tell GL to only draw onto a pixel if the shape is closer to the viewer
    than anything already drawn at that pixel */
    glEnable( GL_DEPTH_TEST ); /* enable depth-testing */
    /* with LESS depth-testing interprets a smaller depth value as meaning "closer" */
    glDepthFunc( GL_LESS );


    // Initialize camera
    camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -26.0f);
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true; 

    // initialize our scene_root
    scene_root = new Node();
}

void Viewer::initShadowMap()
{

    glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Viewer::run()
{
    // sunlight position
    glm::vec3 lightPos(-20.0f, 50.0f, -20.0f);

    while (!glfwWindowShouldClose(win))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (update_callback) {
            update_callback();
        }
        this->process_input(deltaTime);

        Shader* shader = ResourceManager::GetShader("standard"); 

        glm::mat4 lightProjection = glm::ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 100.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        glm::mat4 model = glm::mat4(1.0f);

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        if(shader) {
            glUseProgram(shader->get_id());
            glUniform1i(glGetUniformLocation(shader->get_id(), "isShadowPass"), true);
        }

        glCullFace(GL_FRONT);

        scene_root->draw(model, lightView, lightProjection);

        glCullFace(GL_BACK);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Normal rendering 
        glViewport(0, 0,Config::SCR_WIDTH,Config::SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(shader) {
            glUseProgram(shader->get_id());
            glUniform1i(glGetUniformLocation(shader->get_id(), "isShadowPass"), false);

            glUniform3fv(glGetUniformLocation(shader->get_id(), "viewPos"), 1, &camera->Position[0]);
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glUniform1i(glGetUniformLocation(shader->get_id(), "shadowMap"), 1);
            glUniformMatrix4fv(glGetUniformLocation(shader->get_id(), "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);
            glUniform3fv(glGetUniformLocation(shader->get_id(), "dirLightPos"), 1, &lightPos[0]);
        }

        camera->UpdatePhysics(deltaTime);
        glm::mat4 view = camera->GetViewMatrix();
        float aspectRatio = (float) Config::SCR_WIDTH / (float) Config::SCR_HEIGHT;
        glm::mat4 projection = camera->GetProjectionMatrix(aspectRatio);

        scene_root->draw(model, view, projection);

        if (draw_ui_callback) {
            draw_ui_callback();
        }

        glfwPollEvents();
        glfwSwapBuffers(win);
    }

}

// keyboard handler
void Viewer::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));

    // update the keymap based on key action
    if (action == GLFW_PRESS) {
        viewer->keymap[key] = true;
    } else if (action == GLFW_RELEASE) {
        viewer->keymap[key] = false;
    }
}

void Viewer::process_input(float deltaTime)
{
    for(const auto& [key, is_pressed] : keymap)
    {
        if(is_pressed)
        {
            switch(key){
                case GLFW_KEY_ESCAPE:
                    glfwSetWindowShouldClose(win, GLFW_TRUE);
                    break;
            }
        }
    }
    
}

// mouse movement handler

void Viewer::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (viewer->firstMouse)
    {
        viewer->lastX = xpos;
        viewer->lastY = ypos;
        viewer->firstMouse = false;
    }

    float xoffset = xpos - viewer->lastX;
    float yoffset = viewer->lastY - ypos; 

    viewer->lastX = xpos;
    viewer->lastY = ypos;

    if (std::abs(xoffset) > 50.0f || std::abs(yoffset) > 50.0f) {
        return; 
    }

    viewer->camera->ProcessMouseMovement(xoffset, yoffset);
}

