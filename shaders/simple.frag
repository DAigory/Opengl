#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;
uniform float shiftMix;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in mat4 View;

out vec4 color;

void main()
{
    //color =  mix(texture(ourTexture, TexCoord), texture(ourTexture2, texCoord2), shiftMix);
     vec3 lightPos = (View * vec4(light.position, 1)).xyz;

     vec3 ambient = light.ambient * material.ambient;

     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - FragPos);
     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = light.diffuse * (diffAngle * material.diffuse);

     vec3 viewDir = normalize(-FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * (spec * material.specular);

     vec3 result = (ambient + diffuseColor + specular);
     color = vec4(result,0);
}