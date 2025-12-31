#include "viewer.h"
#include "triangle.h"
#include "cylinder.h"
#include "node.h"
#include "shader.h"
#include <string>
#include <GLFW/glfw3.h> // Nécessaire pour glfwGetTime

#ifndef SHADER_DIR
#error "SHADER_DIR not defined"
#endif


int main()
{
    Viewer viewer(1920, 1200);
    std::string shader_dir = SHADER_DIR;
    Shader *color_shader = new Shader(shader_dir + "node.vert", shader_dir + "node.frag");

    // Root setup
    glm::mat4 human_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f)) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
    Node* human = new Node(human_mat);

    // Torse

    Shape* torso_shape = new Cylinder(color_shader, 2.5f, 0.5f, 50);
    Node* torso_node = new Node(glm::mat4(1.0f)); 
    torso_node->add(torso_shape);
    human->add(torso_node);

    // Tête

    Shape* head_shape = new Cylinder(color_shader, 1.0f, 0.35f, 50);
    glm::mat4 head_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.7f));
    Node* head_node = new Node(head_mat);
    head_node->add(head_shape);
    torso_node->add(head_node); 

    float arm_len = 1.0f; 
    float leg_len = 2.0f; 

    // Bras gauche

    Shape* left_upper_arm_shape = new Cylinder(color_shader, arm_len, 0.25f, 20);
    glm::mat4 left_upper_mat = glm::translate(glm::mat4(1.0f), glm::vec3(-0.80f, 0.0f, -0.45f));
    Node* left_upper_arm_node = new Node(left_upper_mat);
    left_upper_arm_node->add(left_upper_arm_shape);
    torso_node->add(left_upper_arm_node);

    Shape* left_forearm_shape = new Cylinder(color_shader, arm_len, 0.22f, 20);
    glm::mat4 left_fore_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, arm_len));
    Node* left_forearm_node = new Node(left_fore_mat);
    left_forearm_node->add(left_forearm_shape);
    left_upper_arm_node->add(left_forearm_node); 

    // Bras droit

    Shape* right_upper_arm_shape = new Cylinder(color_shader, arm_len, 0.25f, 20);
    glm::mat4 right_upper_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.80f, 0.0f, -0.45f));
    Node* right_upper_arm_node = new Node(right_upper_mat);
    right_upper_arm_node->add(right_upper_arm_shape);
    torso_node->add(right_upper_arm_node); 

    Shape* right_forearm_shape = new Cylinder(color_shader, arm_len, 0.22f, 20);
    glm::mat4 right_fore_mat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, arm_len));
    Node* right_forearm_node = new Node(right_fore_mat);
    right_forearm_node->add(right_forearm_shape);
    right_upper_arm_node->add(right_forearm_node);

    // Jambe gauche

    Shape* left_thigh_shape = new Cylinder(color_shader, leg_len, 0.2f, 20);
    Node* left_thigh_node = new Node(glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, 0.0f, 1.75f)));
    left_thigh_node->add(left_thigh_shape);
    torso_node->add(left_thigh_node);

    Shape* left_calf_shape = new Cylinder(color_shader, leg_len, 0.15f, 20);
    Node* left_calf_node = new Node(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, leg_len)));
    left_calf_node->add(left_calf_shape);
    left_thigh_node->add(left_calf_node);

    // Jambe droite

    Shape* right_thigh_shape = new Cylinder(color_shader, leg_len, 0.2f, 20);
    Node* right_thigh_node = new Node(glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, 0.0f, 1.75f)));
    right_thigh_node->add(right_thigh_shape);
    torso_node->add(right_thigh_node);

    Shape* right_calf_shape = new Cylinder(color_shader, leg_len, 0.15f, 20);
    Node* right_calf_node = new Node(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, leg_len)));
    right_calf_node->add(right_calf_shape);
    right_thigh_node->add(right_calf_node); 

    // Animation function

    viewer.update_callback = [&]() {
        float t = (float)glfwGetTime(); // Temps écoulé depuis le début du programme en secondes

        float wave = 15.0f * sin(t * 8.0f); // Amplitude de 15 degrés, fréquence de 8 radians par seconde
        
        float left_angle = glm::radians(135.0f + wave); // Angle pour le bras gauche

        glm::mat4 head_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.7f))
            * glm::rotate(glm::mat4(1.0f),(float) (glm::radians(10.0f) * sin(t * 8.0f)), glm::vec3(5.0f, 1.0f, 0.0f)); // Inclinaison de la tête

        head_node->set_transform(head_transform); // Met à jour la transformation de la tête
        
        glm::mat4 left_upper_transform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.80f, 0.0f, -0.45f))
            * glm::rotate(glm::mat4(1.0f), left_angle, glm::vec3(0.0f, -1.0f, 0.0f)); 

        left_upper_arm_node->set_transform(left_upper_transform); // Met à jour la transformation du bras gauche

        float right_angle = glm::radians(-135.0f - wave); // Angle pour le bras droit

        glm::mat4 right_upper_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.80f, 0.0f, -0.45f))
            * glm::rotate(glm::mat4(1.0f), right_angle, glm::vec3(0.0f, -1.0f, 0.0f)); 
            
        right_upper_arm_node->set_transform(right_upper_transform); // Met à jour la transformation du bras droit
    };

    viewer.scene_root->add(human); // Ajoute à la scène
    viewer.run(); // Lancer la boucle principale du viewer
}