#pragma once

#include <glm/glm.hpp>
#include "Camera.h"

class Controller {
    public:
         Controller(Camera* camera_, int width, int height): camera(camera_){
            this->resolution = glm::vec2(width, height);
            this->lastMouse = glm::vec2();
            this->mouseDelta = glm::vec2();
            std::fill_n(this->keys, sizeof(this->keys)/sizeof(this->keys[0]), false);
         }

         void Update(float deltaTime){
            if(this->keys[GLFW_KEY_W])
              	this->camera->ToForward(deltaTime);
            if(this->keys[GLFW_KEY_S])
            	this->camera->ToBack(deltaTime);
            if(this->keys[GLFW_KEY_A])
            	this->camera->ToLeft(deltaTime);
            if(this->keys[GLFW_KEY_D])
                this->camera->ToRight(deltaTime);
            if(this->keys[GLFW_KEY_Q])
                this->camera->ToUp(deltaTime);
            if(this->keys[GLFW_KEY_E])
                this->camera->ToDown(deltaTime);
         }

         void Key_callback(GLFWwindow* window, int key, int scancode, int action, int mode, float deltaTime){
               if(action == GLFW_PRESS)
                     keys[key] = true;
                   else if(action == GLFW_RELEASE)
                     keys[key] = false;
         }
         void Mouse_callback(GLFWwindow* window, double xpos, double ypos, float deltaTime){
                if(this->mouseFirst == true){
                    this->lastMouse = glm::vec2(xpos, ypos);
                    this->mouseFirst = false;
                }
                this->mouseDelta.x = xpos - this->lastMouse.x;
                this->mouseDelta.y = this->lastMouse.y - ypos;
                this->lastMouse = glm::vec2(xpos, ypos);
                this->camera->ProcessMouseMovement(this->mouseDelta.x, this->mouseDelta.y, deltaTime);
         }
         void Scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
                this->camera->ProcessMouseScroll(yoffset);
         }

         glm::vec2  GetMouseDelta(){
                return this->mouseDelta;
         }
    private:
        Camera* camera;
        bool keys[1024];
        glm::vec2 lastMouse;
        glm::vec2 mouseDelta;
        glm::vec2 resolution;
        bool mouseFirst = true;
};