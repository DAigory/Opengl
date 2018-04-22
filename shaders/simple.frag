#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
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

uniform float far_plane;


//uniform samplerCube cubemap;
uniform samplerCube shadowMapCub; //depth cubmap

in VS_OUT {
     vec3 Normal;
     vec3 TangentFragPos;
     vec3 FragPos;
     vec2 TexCoords;
     vec3 TangentDirLight;
     #define NR_POINT_LIGHTS 4
     vec3 TangentPointLights[NR_POINT_LIGHTS];
     vec3 TangentViewPos;
     mat3 TBN;
} vs_in;

out vec4 color;

vec3 CalcDirLight(vec3 normal, vec3 viewDir, float shadow);
vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(int index, vec3 normal, vec3 viewDir, float shadow);

float ratio = 1.00 / 5.42;
float gamma = 2.2;
//float bias = max(0.05 * (1.0 - dot(vs_in.NormalWorld, - dirLight.direction.xyz)), 0.005);


float ShadowCalculation(){
    vec3 fragToLight = vs_in.FragPos - pointLight[0].position.xyz;
    float closestDepth = texture(shadowMapCub, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
     float shadow =  1 - ShadowCalculation();
     vec3 result = vec3(0);
     vec3 tangentViewDir = normalize(vs_in.TangentFragPos - vs_in.TangentViewPos);
     vec3 norm = vec3(texture(material.texture_normal1, vs_in.TexCoords));
     norm = normalize(vs_in.TBN * normalize(norm * 2.0 - 1.0).rgb);

     result += CalcDirLight(norm, tangentViewDir, shadow);
   // for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += CalcPointLight(0, norm, tangentViewDir, shadow);
   //}

     result = pow(result, vec3(1.0 / gamma));
     color = vec4(vec3(result), 1);
}

vec3 CalcProjLight(ProjectLight light, vec3 normal, vec3 viewDir,float shadow){
     vec3 lightPos = projectLight.position.xyz;
     vec3 lightDir = normalize(vs_in.TangentFragPos - lightPos);

     float distance    = length(projectLight.position.xyz - vs_in.TangentFragPos);
     float attenuation = 1.0 / (1.0 + projectLight.linear * distance +
     projectLight.quadratic * (distance * distance));

     float theta     = dot(lightDir, normalize(projectLight.direction.xyz));
     float epsilon   = projectLight.cutOff - projectLight.outerCutOff;
     float intensity = clamp((theta - projectLight.outerCutOff) / epsilon, 0.0, 1.0);

     vec3 ambient = projectLight.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));
     ambient  *= attenuation;

     float diffAngle = max(dot(normal, -lightDir), 0.0);
     vec3 diffuseColor = projectLight.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));

     vec3 halfwayDir = normalize(-lightDir + viewDir);
   //  vec3 reflectDir = reflect(lightDir, norm);
     float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
     vec3 specular = projectLight.specular * (spec );

     diffuseColor  *= attenuation;
     specular *= attenuation;

     return ambient + shadow * (diffuseColor + specular) * intensity;
}

vec3 CalcDirLight(vec3 normal, vec3 viewDir, float shadow){
     vec3 lightDir = normalize(vs_in.TangentDirLight);

     float diffAngle = max(dot(normal, -lightDir), 0.0);

     vec3 reflectDir = normalize(reflect(lightDir, normal));

     float spec = pow(max(dot(-viewDir, reflectDir), 0.0), material.shininess);

     vec3 ambient = dirLight.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));
     vec3 diffuseColor = dirLight.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));
     vec3 specular = dirLight.specular * (spec *  vec3(texture(material.texture_specular1, vs_in.TexCoords)));

     //return vec3(diffuseColor);
     return (ambient + shadow * (diffuseColor + specular));
}

vec3 CalcPointLight(int index, vec3 normal, vec3 tangentViewDir, float shadow){
     PointLight light = pointLight[index];
     vec3 tangentLightPos = pointLight[index].position.xyz;

     float distance    = length(tangentLightPos - vs_in.TangentFragPos) ;
     float attenuation = 1.0 / (1.0 + light.linear * distance +
     light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, vs_in.TexCoords));


     vec3 lightDir = normalize(vs_in.TangentFragPos - tangentLightPos);
     float diffAngle = max(dot(-lightDir, normal), 0.0);
     vec3 diffuseColor = light.diffuse * (diffAngle * vec3(texture(material.texture_diffuse1, vs_in.TexCoords)));

     vec3 reflectDir = reflect(lightDir, normal);
     float spec = pow(max(dot(-tangentViewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * (spec *  vec3( texture(material.texture_specular1, vs_in.TexCoords)));

     ambient  *= attenuation;
     diffuseColor  *= attenuation;
     specular *= attenuation;
     //return vec3(diffuseColor);
     return ambient + shadow * (diffuseColor + specular);
}