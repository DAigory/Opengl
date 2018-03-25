#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform samplerCube screenTexture;

void main()
{
     float depthValue = texture(screenTexture, vec3(TexCoords.xy,1)).x;
     FragColor = vec4(vec3(depthValue), 1.0);
}