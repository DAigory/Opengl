#version 330 core

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 normalMatrix;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec2 texCoords;
    vec3 Normal;
    vec3 pos;
} vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.Normal = normal;
    vs_out.texCoords = texCoords;
    vs_out.pos = position;
}