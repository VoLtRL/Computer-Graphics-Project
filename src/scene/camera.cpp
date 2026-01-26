#include "camera.h"
#include "physicShapeObject.h"
#include <iostream>

Camera::Camera(glm::vec3 center, glm::vec3 up, float yaw, float pitch)
    : PhysicShapeObject(nullptr, center - glm::vec3(0,0,8.0f)),
      Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
      MovementSpeed(10.0f), 
      MouseSensitivity(0.1f), 
      Zoom(60.0f),
      InputAcceleration(40.0f),
      Distance(8.0f)
{
    Damping = 10.0f;
    Mass = 1.0f;
    Friction = 0.0f;
    kinematic = false;
    collisionGroup = CG_NONE;
    collisionMask = CG_NONE;
    name = "Camera";

    Target = center;
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
    return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 1000.0f);
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 direction;
    direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    direction.y = sin(glm::radians(Pitch));
    direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    Front = glm::normalize(direction);

    // calculate the new Right and Up vectors
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));

    Position = Target - (Front * Distance);
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
    
    float speedAdd = InputAcceleration * deltaTime;

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
    if (Mass <= 0.0f) return;

    if (kinematic) {
        Position += Velocity * deltaTime;
    }
    else {
        glm::vec3 localAcceleration = forcesApplied / Mass;
        localAcceleration += Acceleration;

        Velocity += localAcceleration * deltaTime;
        Velocity *= glm::exp(-Damping * deltaTime);

        Position += Velocity * deltaTime;

        forcesApplied = glm::vec3(0.0f);
    }
}

void Camera::SetTarget(glm::vec3 newTarget)
{
    Target = newTarget;
    updateCameraVectors(); 
}