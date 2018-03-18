#version 330 core

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 viewOnlyRotate;

layout (location = 0) in vec3 position;

out vec3 TexCoords;

void main()
{
    TexCoords = position;
    vec4 pos = projection * viewOnlyRotate * vec4(position, 1.0);
    gl_Position = pos.xyww;
}
