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

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
    }

    // Set user pointer for GLFW window to this Viewer instance
    glfwSetWindowUserPointer(win, this);

    // register event handlers
    glfwSetKeyCallback(win, key_callback_static);

    // Mouse movement callback
    glfwSetCursorPosCallback(win, mouse_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // useful message to check OpenGL renderer characteristics
    std::cout << glGetString(GL_VERSION) << ", GLSL "
              << glGetString(GL_SHADING_LANGUAGE_VERSION) << ", Renderer "
              << glGetString(GL_RENDERER) << std::endl;

    // initialize GL by setting viewport and default render characteristics
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);

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

        // clear draw buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Appelle la fonction de mise à jour si elle est définie
        if (update_callback) {
            update_callback();
        }

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

void Viewer::key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));
    viewer->on_key(key);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Viewer::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Viewer* viewer = static_cast<Viewer*>(glfwGetWindowUserPointer(window));

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // Si c'est le premier mouvement détecté, on réinitialise lastX/lastY
    // à la position ACTUELLE de la souris pour éviter le saut.
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

    // PROTECTION : Si le saut est trop grand (bug GLFW), on l'ignore
    if (std::abs(xoffset) > 50.0f || std::abs(yoffset) > 50.0f) {
        return; 
    }

    viewer->camera->ProcessMouseMovement(xoffset, yoffset);
}

void Viewer::on_key(int key)
{
    // 'Q' or 'Escape' quits
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(win, GLFW_TRUE);

    }
    
    // Camera movement (ZQSD keys)
    if (key == GLFW_KEY_W)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (key == GLFW_KEY_S)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (key == GLFW_KEY_A)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (key == GLFW_KEY_D)
        camera->ProcessKeyboard(RIGHT, deltaTime);
}

