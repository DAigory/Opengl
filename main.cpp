#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "Mesh.h"
#include "Controller.h"
#include "Texture.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void caclulate_delta_time();

float shiftMix = 0.1;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

glm::mat4 proj;
int width, height;
float fov = 45;

Controller* controller;

glm::vec3 shiftPos;

int main (int argc, char *argv[])
{
    shiftPos = glm::vec3();
  //Инициализация GLFW
  	glfwInit();
  	//Настройка GLFW
  	//Задается минимальная требуемая версия OpenGL.
  	//Мажорная
  	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  	//Минорная
  	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  	//Установка профайла для которого создается контекст
  	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  	//Выключение возможности изменения размера окна
  	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  	GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
    	std::cout << "Failed to create GLFW window" << std::endl;
    	glfwTerminate();
    	return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disabel cursor

    glfwMakeContextCurrent(window);


    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    Camera camera = Camera(glm::vec3(1, 0.0, 3));
    controller = new Controller(&camera, width, height);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,
        1, 2, 3,
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6* sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // Так как VBO объекта-контейнера уже содержит все необходимые данные, то нам нужно только связать с ним новый VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Настраиваем атрибуты (нашей лампе понадобятся только координаты вершин)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindVertexArray(0);

    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);

    Shader shaderSimple = Shader("shaders/simple.vert", "shaders/simple.frag");
    Shader shaderLamp = Shader("shaders/simple.vert", "shaders/lamp.frag");

    GLuint texture = Texture::Load("boxWood.png");
    GLuint texture2 = Texture::Load("boxWoodSpecular.png");
    GLuint texture3 = Texture::Load("matrix.jpg");

    proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST); //enable check z buffer

    glm::vec3 cubePositions[] = {
      glm::vec3( 0.0f,  0.0f,  15.0f),
      glm::vec3( 2.0f,  5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -15.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3( 1.3f, -2.0f, -15.5f),
      glm::vec3( 1.5f,  2.0f, -15.5f),
      glm::vec3( 1.5f,  0.2f, -1.5f),
    };
    caclulate_delta_time();

    glm::vec3 lightPos(0.0f,  0.8f, -1.5f);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        controller->Update(deltaTime);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view;
        view = camera.GetViewMatrix();
        glm::vec3 cameraPosition = camera.Position;
        //cubes
        GLfloat timeValue = glfwGetTime();
        shaderSimple.Use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shaderSimple.SetValue("material.diffuse", 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        shaderSimple.SetValue("material.specular", 1);

        shaderSimple.SetValue("material.shininess", 32.0f);

        shaderSimple.SetValue("light.ambient", 0.2f, 0.2f, 0.2f);
        shaderSimple.SetValue("light.diffuse", 0.5f, 0.5f, 0.5f);
        shaderSimple.SetValue("light.specular", 01.0f, 1.0f, 1.0f);
        shaderSimple.SetValue("light.position", lightPos);

        shaderSimple.SetValue("viewPos", cameraPosition);
        shaderSimple.SetValue("shiftMix", shiftMix);

        glBindVertexArray(VAO);
       // glDrawElements(GL_TRIANGLES, 16, GL_UNSIGNED_INT, 0);

        shaderSimple.SetValue("view", view);
        shaderSimple.SetValue("projection", proj);

        for(GLuint i = 0; i < 7; i++)
        {
          glm::mat4 model;
          glm::vec3 newPos = cubePositions[i] + shiftPos;
          model = glm::translate(model, newPos);
          GLfloat angle = timeValue * glm::radians(5.0f) * (i + 1);
         // model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
          glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
          shaderSimple.SetValue("model", model);
          shaderSimple.SetValue("normalMatrix", normalMatrix);
          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        GLfloat angle = glm::radians(1.1f) * deltaTime * 30;
        glm::mat4 rotate;
        rotate = glm::rotate(rotate, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        lightPos = glm::vec4(lightPos.x, lightPos.y,lightPos.z,0) * rotate;

        //lamp
        glm::mat4 model = glm::mat4();
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        shaderLamp.Use();
        shaderLamp.SetValue("view", view);
        shaderLamp.SetValue("projection", proj);
        shaderLamp.SetValue("model", model);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(model));
        shaderLamp.SetValue("normalMatrix", normalMatrix);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        caclulate_delta_time();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}


void caclulate_delta_time(){
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    controller->Key_callback(window, key, scancode, action, mode, deltaTime);
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    	glfwSetWindowShouldClose(window, GL_TRUE);
    if(key == GLFW_KEY_LEFT && action == GLFW_REPEAT  ){
        shiftMix += 0.01;
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_REPEAT  ){
            shiftMix -= 0.01;
    }

    if(key == GLFW_KEY_I){
        shiftPos.y += 0.1;
    }
    if(key == GLFW_KEY_K){
            shiftPos.y  -= 0.1;
        }
        if(key == GLFW_KEY_J){
                shiftPos.x  -= 0.1;
            }
            if(key == GLFW_KEY_L){
                    shiftPos.x  += 0.1;
                }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

    controller->Mouse_callback(window, xpos, ypos, deltaTime);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    controller->Scroll_callback(window, xoffset, yoffset);
    if(fov >= 1.0f && fov <= 45.0f)
    	fov -= yoffset * deltaTime * 200;
    if(fov <= 1.0f)
    	fov = 1.0f;
    if(fov >= 45.0f)
  	fov = 45.0f;

  	proj = glm::perspective(glm::radians(fov), (float)width/(float)height, 0.1f, 100.0f);
}

