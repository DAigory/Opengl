#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out mat4 View;
out vec2 TexCoords;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = (view * model * vec4(position, 1.0)).xyz;
    Normal = (view * vec4(normalMatrix * normal, 0)).xyz;
    View = view;
    TexCoords = texCoords;
}