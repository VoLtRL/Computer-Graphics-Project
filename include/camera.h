#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Velocity;
    float Acceleration;     
    float Damping;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    glm::vec3 Target;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
    float Distance;

    Camera(glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspectRatio);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void UpdatePhysics(float deltaTime);
    void SetTarget(glm::vec3 newTarget);

private:
    void updateCameraVectors();
};