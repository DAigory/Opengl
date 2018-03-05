#version 330 core

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float shiftMix;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in mat4 View;

out vec4 color;


void main()
{
    //color =  mix(texture(ourTexture, TexCoord), texture(ourTexture2, texCoord2), shiftMix);
     vec3 lightPos = (View * vec4(lightPos, 1)).xyz;
     float ambientStrength = 0.1f;
     vec3 ambient = ambientStrength * lightColor;

     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - FragPos);
     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = diffAngle * lightColor;

     float specularStrength = 0.5f;
     vec3 viewDir = normalize(FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
     vec3 specular = specularStrength * spec * lightColor;

     vec3 result = (ambient + diffuseColor + specular) * objectColor;
     color = vec4(result, 1);
}