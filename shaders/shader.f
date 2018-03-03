#version 330 core

uniform vec3 ourColor;
uniform float shiftMix;

in vec3 vertexColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main()
{
    vec2 texCoord2 = vec2(TexCoord.x, 1.0f - TexCoord.y);
    color =  mix(texture(ourTexture, TexCoord), texture(ourTexture2, texCoord2), shiftMix);
};