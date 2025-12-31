#ifndef VIEWER_H
#define VIEWER_H

#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "node.h"

#include <functional>

class Viewer {
public:
    Viewer(int width=640, int height=480);

    void run();
    void on_key(int key);

    Node *scene_root;

    std::function<void()> update_callback; // Fonction de rappel pour les mises à jour par frame

private:
    GLFWwindow* win;
    static void key_callback_static(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif // VIEWER_H
