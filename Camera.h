#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  10.25f;
const GLfloat ZOOM       =  45.0f;

class Camera {
public:
        // Camera Attributes
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // Eular Angles
        GLfloat Yaw;
        GLfloat Pitch;
        // Camera options
        GLfloat MovementSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
        {
            this->Position = position;
            this->WorldUp = up;
            this->Yaw = yaw;
            this->Pitch = pitch;
            this->updateCameraVectors();
        }

        glm::mat4 GetViewMatrix()
        {
              return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
        }

        void ToLeft(float deltaTime){
            this->Position -= this->Right * this->GetVelocity(deltaTime);
        }

        void ToRight(float deltaTime){
             this->Position += this->Right * this->GetVelocity(deltaTime);
        }

        void ToForward(float deltaTime){
             this->Position += this->Front * this->GetVelocity(deltaTime);
        }

        void ToBack(float deltaTime){
             this->Position -= this->Front * this->GetVelocity(deltaTime);
        }

        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, float deltaTime, GLboolean constrainPitch = true)
        {
            xoffset *= this->MouseSensitivity * deltaTime;
            yoffset *= this->MouseSensitivity * deltaTime;

            this->Yaw   += xoffset;
            this->Pitch += yoffset;


            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (this->Pitch > 89.0f)
                    this->Pitch = 89.0f;
                if (this->Pitch < -89.0f)
                    this->Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Eular angles
            this->updateCameraVectors();
        }

        void ProcessMouseScroll(GLfloat yoffset)
        {
            if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
                this->Zoom -= yoffset;
            if (this->Zoom <= 1.0f)
                this->Zoom = 1.0f;
            if (this->Zoom >= 45.0f)
                this->Zoom = 45.0f;
         }
private:
    float GetVelocity(float deltaTime){
        return this->MovementSpeed * deltaTime;
    }
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        this->Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->Up    = glm::normalize(glm::cross(this->Right, this->Front));
    }
};