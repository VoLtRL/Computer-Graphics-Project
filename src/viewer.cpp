#include "viewer.h"
#include "camera.h"

#include <iostream>
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>


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
    camera = new Camera(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -20.0f);
    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true; 

    // initialize our scene_root
    scene_root = new Node();
}

void Viewer::run()
{
    // Main render loop for this OpenGL window
    while (!glfwWindowShouldClose(win))
    {

        float currentFrame = (float)glfwGetTime();

        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (update_callback) {
            update_callback();
        }
        // Process input
        this->process_input(deltaTime);
        

        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = camera->GetViewMatrix();

        float aspectRatio = (float) SCR_WIDTH / (float) SCR_HEIGHT;
        glm::mat4 projection = camera->GetProjectionMatrix(aspectRatio);

        camera->UpdatePhysics(deltaTime);

        scene_root->draw(model, view, projection);


        // Poll for and process events
        glfwPollEvents();

        // flush render commands, and swap draw buffers
        glfwSwapBuffers(win);
    }

    /* close GL context and any other GLFW resources */
    glfwTerminate();
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
                case GLFW_KEY_W:
                    camera->ProcessKeyboard(FORWARD, deltaTime);
                    break;
                case GLFW_KEY_S:
                    camera->ProcessKeyboard(BACKWARD, deltaTime);
                    break;
                case GLFW_KEY_A:
                    camera->ProcessKeyboard(LEFT, deltaTime);
                    break;
                case GLFW_KEY_D:
                    camera->ProcessKeyboard(RIGHT, deltaTime);
                    break;
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

