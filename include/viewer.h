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

class Viewer {
public:
    Viewer(int width=640, int height=480);

    float deltaTime = 0.0f; 
    float lastFrame = 0.0f;

    std::map<int, bool> keymap = {{GLFW_KEY_W, false}, {GLFW_KEY_A, false}, {GLFW_KEY_S, false}, {GLFW_KEY_D, false}, {GLFW_KEY_ESCAPE, false}, {GLFW_KEY_SPACE, false}, {GLFW_KEY_F, false}, {GLFW_MOUSE_BUTTON_LEFT, false}};
    unsigned int depthMapFBO;
    unsigned int depthMap;
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    void run();
    void on_key(int key);
    void initShadowMap();


    Node *scene_root;
    Camera* camera;

    std::function<void()> update_callback; // Fonction de rappel pour les mises à jour par frame
    std::function<void()> draw_ui_callback; // Draw UI callback

    glm::vec3 backgroundColor = glm::vec3(0.2f, 0.2f, 0.2f);

private:
    GLFWwindow* win;

    float lastX;
    float lastY;
    bool firstMouse;
    
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void process_input(float deltaTime);
};

#endif // VIEWER_H
