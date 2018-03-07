#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

uniform Light light;
uniform Material material;
uniform float shiftMix;

uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in mat4 View;
in vec2 TexCoords;

out vec4 color;

void main()
{
    //color =  mix(texture(ourTexture, TexCoord), texture(ourTexture2, texCoord2), shiftMix);
     vec3 lightPos = light.position.xyz;

     float distance    = length(light.position.xyz - FragPos);
     float attenuation = 1.0 / (1.0 + light.linear * distance +
     light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));


     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - FragPos);
     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.diffuse, TexCoords)));

     vec3 viewDir = normalize(-FragPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * (spec *  vec3( texture(material.specular, TexCoords)));

     ambient  *= attenuation;
     diffuseColor  *= attenuation;
     specular *= attenuation;

     vec3 result = (ambient  + diffuseColor + specular);
     color = vec4(result, 1);
}