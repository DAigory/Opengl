#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include "shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main (int argc, char *argv[])
{
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

  	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
    	std::cout << "Failed to create GLFW window" << std::endl;
    	glfwTerminate();
    	return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glfwSetKeyCallback(window, key_callback);

    GLfloat vertices[] = {
              0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Нижний правый угол
              -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // Нижний левый угол
              0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Верхний угол
        };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 2,  // First Triangle
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    Shader shader = Shader("shaders/shader.v", "shaders/shader.f");
    GLint vertexColorLocation = glGetUniformLocation(shader.GetProgram(), "ourColor");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        GLfloat timeValue = glfwGetTime();
        GLfloat greenValue = (sin(timeValue) / 2) + 0.5;
        shader.Use();
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    	glfwSetWindowShouldClose(window, GL_TRUE);
}