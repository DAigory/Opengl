#version 330 core


struct DirLight {
    vec4 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec4 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float linear;
    float quadratic;
};

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;
uniform int normalDir;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 biTangent;

out VS_OUT {
    vec3 Normal;
    vec3 TangentFragPos;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentDirLight;
    #define NR_POINT_LIGHTS 4
    vec3 TangentPointLights[NR_POINT_LIGHTS];
    vec3 TangentViewPos;
} vs_out;

void LightsTBN(mat3 TBN ){
    for(int i = 0; i < NR_POINT_LIGHTS; i++){
        vs_out.TangentPointLights[i] = TBN * pointLight[i].position.xyz;
    }
    vs_out.TangentDirLight = TBN * normalize( dirLight.direction.xyz);
}

void main()
{
    vs_out.Normal = normal * normalDir;
    vec3 T = normalize(vec3(model * vec4(tangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(vs_out.Normal,    0.0)));
    vec3 B = normalize(vec3(model * vec4(biTangent,    0.0)));

    mat3 TBN = transpose(mat3(T, B, N));
    LightsTBN(TBN);
    gl_Position = projection * view * model * vec4(position, 1.0);
    vs_out.FragPos = (model * vec4(position, 1.0)).xyz;
    vs_out.TangentFragPos = TBN * vec3(model * vec4(position, 1.0));
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TexCoords = texCoords;

}

