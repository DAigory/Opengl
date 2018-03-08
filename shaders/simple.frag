#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct ProjectLight{
    vec4 position;
    vec4  direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

struct PointLight {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};


struct DirLight {
    vec4 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;
uniform ProjectLight projectLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform Material material;
uniform float shiftMix;

uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in mat4 View;
in vec2 TexCoords;

out vec4 color;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main()
{
    //color =  mix(texture(ourTexture, TexCoord), texture(ourTexture2, texCoord2), shiftMix);
//     vec3 lightPos = projectLight.position.xyz;
//     vec3 lightDir = normalize(lightPos - FragPos);
//
//     float distance    = length(projectLight.position.xyz - FragPos);
//     float attenuation = 1.0 / (1.0 + projectLight.linear * distance +
//     projectLight.quadratic * (distance * distance));
//
//     float theta     = dot(lightDir, normalize(-projectLight.direction.xyz));
//     float epsilon   = projectLight.cutOff - projectLight.outerCutOff;
//     float intensity = clamp((theta - projectLight.outerCutOff) / epsilon, 0.0, 1.0);
//
//     vec3 ambient = projectLight.ambient * vec3(texture(material.diffuse, TexCoords));
//     ambient  *= attenuation;
//
//     vec3 norm = normalize(Normal);
//
//     float diffAngle = max(dot(norm, lightDir), 0.0);
//     vec3 diffuseColor = projectLight.diffuse * (diffAngle * vec3(texture(material.diffuse, TexCoords)));
//
//     vec3 viewDir = normalize(-FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
//     vec3 specular = projectLight.specular * (spec *  vec3( texture(material.specular, TexCoords)));
//
//     diffuseColor  *= attenuation;
//     specular *= attenuation;
//
//     vec3 result = ambient+ (diffuseColor + specular) * intensity;
     vec3 result = vec3(0);
     vec3 viewDir = normalize(-FragPos);
     vec3 normal = normalize(Normal);
     result += CalcProjLight(projectLight, normal, viewDir);
     result += CalcDirLight(dirLight, normal, viewDir);
     for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLight[i], normal, viewDir);
     }
     color = vec4(result,1);
}

vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir){
     vec3 lightPos = projectLight.position.xyz;
     vec3 lightDir = normalize(lightPos - FragPos);

     float distance    = length(projectLight.position.xyz - FragPos);
     float attenuation = 1.0 / (1.0 + projectLight.linear * distance +
     projectLight.quadratic * (distance * distance));

     float theta     = dot(lightDir, normalize(-projectLight.direction.xyz));
     float epsilon   = projectLight.cutOff - projectLight.outerCutOff;
     float intensity = clamp((theta - projectLight.outerCutOff) / epsilon, 0.0, 1.0);

     vec3 ambient = projectLight.ambient * vec3(texture(material.diffuse, TexCoords));
     ambient  *= attenuation;

     vec3 norm = normalize(Normal);

     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = projectLight.diffuse * (diffAngle * vec3(texture(material.diffuse, TexCoords)));

     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = projectLight.specular * (spec *  vec3( texture(material.specular, TexCoords)));

     diffuseColor  *= attenuation;
     specular *= attenuation;

     return ambient + (diffuseColor + specular) * intensity;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
     vec3 lightDir = (View * light.direction).xyz;

     float diffAngle = max(dot(normal, lightDir), 0.0);

     vec3 reflectDir = normalize(reflect(-lightDir, normal));

     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.diffuse, TexCoords)));
     vec3 specular = light.specular * (spec *  vec3( texture(material.specular, TexCoords)));

     return (ambient + diffuseColor + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir){
     vec3 lightPos = light.position.xyz;

     float distance    = length(light.position.xyz - FragPos);
     float attenuation = 1.0 / (1.0 + light.linear * distance +
     light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - FragPos);
     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.diffuse, TexCoords)));

     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * (spec *  vec3( texture(material.specular, TexCoords)));

     ambient  *= attenuation;
     diffuseColor  *= attenuation;
     specular *= attenuation;

     return (ambient  + diffuseColor + specular);
}