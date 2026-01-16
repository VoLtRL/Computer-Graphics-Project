#include "camera.h"

Camera::Camera(glm::vec3 center, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(10.0f), 
      MouseSensitivity(0.1f), 
      Zoom(45.0f),
      Velocity(glm::vec3(0.0f)), 
      Acceleration(40.0f), 
      Damping(45.0f),
      Distance(15.0f)
{
    Target = center;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}
glm::mat4 Camera::GetViewMatrix()
    {
        return glm::lookAt(Position, Target, Up); // Calculate view matrix using LookAt matrix
    }

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio)
    {
        return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 200.0f); // Perspective projection matrix
    }

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 direction;
    direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    direction.y = sin(glm::radians(Pitch));
    direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    Front = glm::normalize(direction);

    // update Position based on Target and Distance
    Position = Target - (Front * Distance);

    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));
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

void Camera::SetTarget(glm::vec3 newTarget)
{
    Target = newTarget;
    updateCameraVectors(); // Calculate new Position based on updated Target
}