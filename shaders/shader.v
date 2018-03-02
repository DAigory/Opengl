#version 330 core

out vec3 vertexColor;
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;


void main()
{
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    vertexColor = color;
};