#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Controller.h"
#include "Model.h"
#include "TextureLoader.h"

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
 std::cout << "1 " << std::endl;
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
  	glfwWindowHint(GLFW_SAMPLES, 4); //for msaa 4 data to pixel

  	glEnable(GL_MULTISAMPLE);
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

    Camera camera = Camera(glm::vec3(0, 0, 6));
    controller = new Controller(&camera, width, height);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    glEnable(GL_DEPTH_TEST);

    caclulate_delta_time();

    //glm::vec4 lightPos(0.0f,  0.8f, -1.5f, 1.0f);
    glm::vec4 lightDir(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 direction(-1.0f, 0.0f, 0.0f, 0.0f);

    float cutOff = 35;
    float outerCutOff = 45;

    glm::vec4 pointLightPositions[] = {
    	glm::vec4( 0.7f,  0.2f,  2.0f, 1.0f),
    	glm::vec4( 2.3f, -3.3f, -4.0f, 1.0f),
    	glm::vec4(-4.0f,  2.0f, -12.0f, 1.0f),
    	glm::vec4( 0.0f,  0.0f, -3.0f, 1.0f)
    };

    proj = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);
    GLuint cubMapTexture = TextureLoader::LoadCubMap("sky", "assets/cubmap/skybox");

    Shader shaderSimple = Shader("shaders/simple.vert", "shaders/simple.frag");
    Shader shaderSimple2 = Shader("shaders/simple.vert", "shaders/simple.frag");
    Shader shaderLamp = Shader("shaders/simple.vert", "shaders/lamp.frag");
    Shader shaderNormal = Shader("shaders/normals/normal.vert", "shaders/normals/constColor.frag", "shaders/normals/normal.geom");
    Shader shaderCubMap = Shader("shaders/skybox.vert", "shaders/skybox.frag");
    Shader depthShader = Shader("shaders/depth/lightView.vert", "shaders/depth/empty.frag");


    Model soldierModel("assets/soldier/nanosuit.obj");
    Model cubeModel("assets/primitives/cub.obj");
    Model planeModel("assets/primitives/plane.obj");
    Model quadModel("assets/primitives/quad.obj");

    shaderSimple.BindUniformBlock("Matrices", 0);
    shaderCubMap.BindUniformBlock("Matrices", 0);
    shaderNormal.BindUniformBlock("Matrices", 0);
    shaderSimple2.BindUniformBlock("Matrices", 0);
    shaderLamp.BindUniformBlock("Matrices", 0);

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(proj));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


   // configure depth map FBO
       // -----------------------
       const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
       unsigned int depthMapFBO;
       glGenFramebuffers(1, &depthMapFBO);
       // create depth cubemap texture
       unsigned int depthCubemap;
       glGenTextures(1, &depthCubemap);
       glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
       for (unsigned int i = 0; i < 6; ++i)
           glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
       glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
       // attach depth texture as FBO's depth buffer
       glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
       glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
       glDrawBuffer(GL_NONE);
       glReadBuffer(GL_NONE);
       glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float far = 50.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH/(float)SHADOW_HEIGHT, 0.1f, far);

    glm::vec3 lightPos = glm::vec3(1.5, 1.2, 1);

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
                     glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));

    Shader depthShaderCub = Shader("shaders/cubMapDepth/model.vert"
                                ,"shaders/cubMapDepth/depthCubMap.frag"
                                ,"shaders/cubMapDepth/cubMapToLayer.geom");
    Shader screenShader = Shader("shaders/DrawToQuad/noMatrix.vert", "shaders/DrawToQuad/texture.frag");

    //GLuint textNormal = TextureLoader::Load("brickwall_normal.jpg", "assets/textures/");

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        controller->Update(deltaTime);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 view;
        view = camera.GetViewMatrix();

        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        GLfloat timeValue = glfwGetTime();

        glm::mat4 modelPrimitive;
        glm::vec3 pos = glm::vec3(2.0f, -2.0f, 1.0);
        modelPrimitive = glm::translate(modelPrimitive, pos);

        glm::mat4 modelSoldier;
        glm::vec3 newPosSoldier =  glm::vec3(4.0f, 2.0f, 2.0);
        modelSoldier = glm::translate(modelSoldier, newPosSoldier);
        modelSoldier = glm::scale(modelSoldier, glm::vec3(0.2f));

        glm::mat4 modelCubMat = glm::mat4();
        modelCubMat = glm::translate(modelCubMat, glm::vec3(0,0,0));
        modelCubMat = glm::scale(modelCubMat, glm::vec3(7));

        //draw to cub depth buffer
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glCullFace(GL_FRONT);
        depthShaderCub.Use();

        glEnable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glDrawBuffer(GL_NONE);//doesn't render to a color buffer
        glReadBuffer(GL_NONE);



         glm::vec3 lightPos = glm::vec3(1.5, 1.2, 1) + shiftPos;

            std::vector<glm::mat4> shadowTransforms;
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
            shadowTransforms.push_back(shadowProj *
                             glm::lookAt(lightPos, lightPos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));


        for (unsigned int i = 0; i < 6; ++i)
        {
            depthShaderCub.SetValue(("shadowMatrices[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }
        depthShaderCub.SetValue("far_plane", far);
        depthShaderCub.SetValue("lightPos", lightPos);
        depthShaderCub.SetValue("model", modelSoldier);
        cubeModel.Draw(depthShaderCub);
        depthShaderCub.SetValue("model", modelCubMat);
        cubeModel.Draw(depthShaderCub);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //draw to screen buffer
                glViewport(0, 0, width, height);
                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                shaderSimple.Use();
                shaderSimple.SetValue("viewPos",camera.Position);
//                glActiveTexture(GL_TEXTURE3);
//                shaderSimple.SetValue("texture_normal",3);
//                glBindTexture(GL_TEXTURE_2D, textNormal);
                glActiveTexture(GL_TEXTURE4);
                shaderSimple.SetValue("shadowMapCub",4);
                glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
                // glActiveTexture(GL_TEXTURE0+1);
                // shaderSimple.SetValue("cubemap",1);
               //  glBindTexture(GL_TEXTURE_CUBE_MAP, cubMapTexture);
                 shaderSimple.SetValue("far_plane", far);
                 shaderSimple.SetValue("dirLight.ambient", 0.1f, 0.1f, 0.1f);
                 shaderSimple.SetValue("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
                 shaderSimple.SetValue("dirLight.specular", 0.1f, 0.1f, 0.1f);
                 shaderSimple.SetValue("dirLight.direction", 1,0,0,0);

//
//       shaderSimple.SetValue("projectLight.ambient", 0.03f, 0.03f, 0.03f);
//       shaderSimple.SetValue("projectLight.diffuse", 1.0f, 1.0f, 1.0f);
//       shaderSimple.SetValue("projectLight.specular", 1.0f, 1.0f, 1.0f);
//       shaderSimple.SetValue("projectLight.position", view * glm::vec4(0,2,-7,1));
//       shaderSimple.SetValue("projectLight.direction", view * glm::vec4(0,-1,-1.5,0));
//       shaderSimple.SetValue("projectLight.cutOff",  glm::cos(glm::radians(cutOff)));
//       shaderSimple.SetValue("projectLight.outerCutOff",  glm::cos(glm::radians(outerCutOff)));
//       shaderSimple.SetValue("projectLight.linear",    0.09f);
//       shaderSimple.SetValue("projectLight.quadratic", 0.032f);
//

             std::string pointLight = "pointLight[";
             pointLight += std::to_string(0) + "].";
             shaderSimple.SetValue((pointLight + "ambient").c_str(), 0.1f, 0.1f, 0.1f);
             shaderSimple.SetValue((pointLight + "diffuse").c_str(), 0.3f, 0.3f, 0.3f);
             shaderSimple.SetValue((pointLight + "specular").c_str(), 0.1f, 0.1f, 0.1f);
             shaderSimple.SetValue((pointLight + "position").c_str(), glm::vec4(lightPos.x,lightPos.y,lightPos.z,1));
             shaderSimple.SetValue((pointLight + "linear").c_str(),    0.09f);
             shaderSimple.SetValue((pointLight + "quadratic").c_str(), 0.032f);

             shaderSimple.SetValue("model", modelPrimitive);
             //planeModel.Draw(shaderSimple);

             shaderSimple.SetValue("material.shininess", 32.0f);

//       // std::cout << "x: "<< a.x << "y: " << a.y << "z:  "<< a.z << std::endl;
//
//        shaderSimple.SetValue("projectLight.ambient", 0.03f, 0.03f, 0.03f);
//        shaderSimple.SetValue("projectLight.diffuse", 1.0f, 1.0f, 1.0f);
//        shaderSimple.SetValue("projectLight.specular", 1.0f, 1.0f, 1.0f);
//        shaderSimple.SetValue("projectLight.position", glm::vec4(0,0,0,1));
//        shaderSimple.SetValue("projectLight.direction", view * camera.GetFront4());
//        shaderSimple.SetValue("projectLight.cutOff",  glm::cos(glm::radians(cutOff)));
//        shaderSimple.SetValue("projectLight.outerCutOff",  glm::cos(glm::radians(outerCutOff)));
//        shaderSimple.SetValue("projectLight.linear",    0.09f);
//        shaderSimple.SetValue("projectLight.quadratic", 0.032f);



                      GLfloat angle = timeValue * glm::radians(5.0f);
                      // model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
                      glm::mat3 normalMatrix = glm::transpose(glm::inverse(modelSoldier));
                      shaderSimple.SetValue("model", modelSoldier);
                      shaderSimple.SetValue("normalMatrix", normalMatrix);
                      shaderSimple.SetValue("shiftMix", shiftMix);
                      shaderSimple.SetValue("normalDir", 1);
                      cubeModel.Draw(shaderSimple);
                     // soldierModel.Draw(shaderSimple);
                       shaderSimple.SetValue("normalDir", -1);
                       shaderSimple.SetValue("model", modelCubMat);
                       cubeModel.Draw(shaderSimple);


                       shaderLamp.Use();
                       modelCubMat = glm::mat4();
                       modelCubMat = glm::translate(modelCubMat, lightPos);
                       modelCubMat = glm::scale(modelCubMat, glm::vec3(0.1f));
                       shaderLamp.SetValue("model", modelCubMat);
                       cubeModel.Draw(shaderLamp);
//
//         //lamp data to shader
//         int count = sizeof(pointLightPositions) / sizeof(*pointLightPositions);
//
//         angle = glm::radians(1.1f) * deltaTime * 30;
//         glm::mat4 rotate;
//         rotate = glm::rotate(rotate, angle, glm::vec3(0.0f, 1.0f, 0.0f));
//
//         shaderSimple.SetValue("time", float(glfwGetTime()));
//
//         for(int i =0;i<count;i++){
//             glm::vec3 newPos =  pointLightPositions[i]*rotate;
//             pointLightPositions[i] = glm::vec4(newPos.x, newPos.y,newPos.z, 1.0f);
//             glm::vec3 position3 = pointLightPositions[i];
//             glm::vec4 position = glm::vec4(position3.x, position3.y, position3.z, 1);
//             std::string pointLight = "pointLight[";
//             pointLight += std::to_string(i) + "].";
//             shaderSimple.SetValue((pointLight + "ambient").c_str(), 0.03f, 0.03f, 0.03f);
//             shaderSimple.SetValue((pointLight + "diffuse").c_str(), 0.5f, 0.5f, 0.5f);
//             shaderSimple.SetValue((pointLight + "specular").c_str(), 01.0f, 1.0f, 1.0f);
//             shaderSimple.SetValue((pointLight + "position").c_str(), view * position);
//             shaderSimple.SetValue((pointLight + "linear").c_str(),    0.09f);
//             shaderSimple.SetValue((pointLight + "quadratic").c_str(), 0.032f);
//         }
//
//         //lamp draw like cubes
//         shaderLamp.Use();
//         shaderLamp.SetValue("view", view);
//         shaderLamp.SetValue("projection", proj);
//
//         for(int i =0; i<count; i++){
//           glm::vec4 position = pointLightPositions[i];
//           glm::mat4 model = glm::mat4();
//           model = glm::translate(model, glm::vec3(position));
//           model = glm::scale(model, glm::vec3(0.2f));
//           shaderLamp.SetValue("model", model);
//           cubeModel.Draw(shaderLamp);
//         }
//
//         //skybox
//         glDepthFunc(GL_LEQUAL);
//         shaderCubMap.Use();
//         glm::mat4 viewOnlyRotate = glm::mat4(glm::mat3(view));
//         shaderCubMap.SetValue("viewOnlyRotate", viewOnlyRotate);
//         shaderCubMap.SetValue("skybox", 0);
//         glBindTexture(GL_TEXTURE_CUBE_MAP, cubMapTexture);
//         //cubeModel.Draw(shaderCubMap);
//         glDepthFunc(GL_LESS);

//render texture to screen quad
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(0, 0, width, height);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        screenShader.Use();
//        glDisable(GL_DEPTH_TEST);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//        quadModel.Draw(screenShader);

         glfwSwapBuffers(window);
         caclulate_delta_time();
    }

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
        shiftPos.z -= 0.1;
    }
    if(key == GLFW_KEY_K){
            shiftPos.z  += 0.1;
        }
    if(key == GLFW_KEY_J){
            shiftPos.x  -= 0.1;
    }
    if(key == GLFW_KEY_L){
                shiftPos.x  += 0.1;
    }
     if(key == GLFW_KEY_U){
                shiftPos.y  += 0.1;
        }
        if(key == GLFW_KEY_O){
                    shiftPos.y  -= 0.1;
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

