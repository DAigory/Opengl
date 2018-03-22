#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
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
uniform samplerCube cubemap;
uniform sampler2D shadowMap; //depth map (build by view from light position)


in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec3 NormalWorld;
    vec2 TexCoords;
    mat4 View;
    vec4 FragPosLightSpace;
} vs_in;

out vec4 color;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow);

float ratio = 1.00 / 5.42;
float gamma = 2.2;
float bias = max(0.05 * (1.0 - dot(vs_in.NormalWorld, - dirLight.direction.xyz)), 0.005);

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z - bias;
    float shadow = currentDepth >= closestDepth  ? 1.0 : 0.0;
    return shadow;
}

void main()
{
     float shadow = 1 - ShadowCalculation(vs_in.FragPosLightSpace);
     vec3 result = vec3(0);
     vec3 viewDir = normalize(-vs_in.FragPos);
     vec3 normal = normalize(vs_in.Normal);
     result += CalcProjLight(projectLight, normal, viewDir, shadow);
     result += CalcDirLight(dirLight, normal, viewDir, shadow);
     for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(pointLight[i], normal, viewDir, shadow);
     }
     vec3 spec = vec3(texture(material.texture_diffuse1, vs_in.TexCoords));
     vec3 R = refract(viewDir, normalize(vs_in.Normal), ratio);
     vec3 cubMapColor = texture(cubemap, R).rgb;

     result = pow(result, vec3(1.0 / gamma));
     color = vec4(result.xyz, 1);
}

vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir,float shadow){
     vec3 lightPos = projectLight.position.xyz;
     vec3 lightDir = normalize(vs_in.FragPos - lightPos);

     float distance    = length(projectLight.position.xyz - vs_in.FragPos);
     float attenuation = 1.0 / (1.0 + projectLight.linear * distance +
     projectLight.quadratic * (distance * distance));

     float theta     = dot(lightDir, normalize(projectLight.direction.xyz));
     float epsilon   = projectLight.cutOff - projectLight.outerCutOff;
     float intensity = clamp((theta - projectLight.outerCutOff) / epsilon, 0.0, 1.0);

     vec3 ambient = projectLight.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));
     ambient  *= attenuation;

     vec3 norm = normalize(vs_in.Normal);

     float diffAngle = max(dot(norm, -lightDir), 0.0);
     vec3 diffuseColor = projectLight.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));

     vec3 halfwayDir = normalize(-lightDir + viewDir);
   //  vec3 reflectDir = reflect(lightDir, norm);
     float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
     vec3 specular = projectLight.specular * (spec );

     diffuseColor  *= attenuation;
     specular *= attenuation;

     return ambient + shadow * (diffuseColor + specular) * intensity;
    //return vec3(specular );
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow){
     vec3 lightDir = (vs_in.View * light.direction).xyz;

     float diffAngle = max(dot(normal, -lightDir), 0.0);

     vec3 reflectDir = normalize(reflect(lightDir, normal));

     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));
     vec3 specular = light.specular * (spec *  vec3(texture(material.texture_specular1, vs_in.TexCoords)));

     return (ambient + shadow *(diffuseColor + specular));
     //return vec3(specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir, float shadow){
     vec3 lightPos = light.position.xyz;

     float distance    = length(light.position.xyz - vs_in.FragPos);
     float attenuation = 1.0 / (1.0 + light.linear * distance +
     light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));

     vec3 norm = normalize(vs_in.Normal);
     vec3 lightDir = normalize(lightPos - vs_in.FragPos);
     float diffAngle = max(dot(norm, lightDir), 0.0);
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));

     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * (spec *  vec3( texture(material.texture_specular1, vs_in.TexCoords)));

     ambient  *= attenuation;
     diffuseColor  *= attenuation;
     specular *= attenuation;

     return ambient + shadow * (diffuseColor + specular);
}