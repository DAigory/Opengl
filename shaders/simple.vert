#version 330 core

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;
uniform int normalDir;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 FragPosWorld;
    vec2 TexCoords;
    mat4 View;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.FragPos = (view * model * vec4(position, 1.0)).xyz;
    vs_out.FragPosWorld = (model * vec4(position, 1.0)).xyz;
    vs_out.Normal = (view * vec4(normalMatrix * normal, 0)).xyz * normalDir;
    vs_out.View = view;
    vs_out.TexCoords = texCoords;
}