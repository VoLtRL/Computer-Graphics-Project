#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(10.0f), 
      MouseSensitivity(0.1f), 
      Zoom(45.0f),
      Velocity(glm::vec3(0.0f)), 
      Acceleration(40.0f), 
      Damping(4.0f)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}
glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio)
    {
        return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 100.0f);
    }

void Camera::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float speedAdd = Acceleration * deltaTime;

    if (direction == FORWARD)
        Velocity += Front * speedAdd;
    if (direction == BACKWARD)
        Velocity -= Front * speedAdd;
    if (direction == LEFT)
        Velocity -= Right * speedAdd;
    if (direction == RIGHT)
        Velocity += Right * speedAdd;
}

void Camera::UpdatePhysics(float deltaTime)
{
    Position += Velocity * deltaTime;

    if (glm::length(Velocity) > 0.0f)
    {
        Velocity -= Velocity * Damping * deltaTime;
        
        if (glm::length(Velocity) > MovementSpeed) 
        {
             Velocity = glm::normalize(Velocity) * MovementSpeed;
        }
    }
}