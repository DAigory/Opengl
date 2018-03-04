#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec3 vertexColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    TexCoord = texCoord;
};