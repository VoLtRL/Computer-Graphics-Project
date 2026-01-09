#ifndef VIEWER_H
#define VIEWER_H

#include <vector>
#include <string>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "node.h"
#include "camera.h"

#include <functional>

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1200;

class Viewer {
public:
    Viewer(int width=640, int height=480);

    float deltaTime = 0.0f; 
    float lastFrame = 0.0f;

    std::map<int, bool> keymap = {{GLFW_KEY_W, false}, {GLFW_KEY_A, false}, {GLFW_KEY_S, false}, {GLFW_KEY_D, false}, {GLFW_KEY_ESCAPE, false}};

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
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    void process_input(float deltaTime);
};

#endif // VIEWER_H
