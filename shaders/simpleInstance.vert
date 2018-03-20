#version 330 core

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in mat4 modelInstance;

out vec3 Normal;
out vec3 FragPos;
out mat4 View;
out vec2 TexCoords;

void main()
{
    vec3 normalMatrix = vec3(transpose(inverse(view * modelInstance)));
    gl_Position = projection * view * modelInstance * vec4(position, 1.0);
    FragPos = (view * modelInstance * vec4(position, 1.0)).xyz;
    Normal = (view * vec4(normalMatrix * normal, 0)).xyz;
    View = view;
    TexCoords = texCoords;
}


