#ifndef VIEWER_H
#define VIEWER_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "node.h"
#include "camera.h"

#include <functional>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 1080;


class Viewer {
public:
    Viewer(int width=640, int height=480);




    float deltaTime = 0.0f; 
    float lastFrame = 0.0f;

    void run();
    void on_key(int key);

    Node *scene_root;

    std::function<void()> update_callback; // Fonction de rappel pour les mises à jour par frame

private:
    GLFWwindow* win;

    Camera* camera; 
    float lastX;
    float lastY;
    bool firstMouse;
    
    static void key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
};

#endif // VIEWER_H
