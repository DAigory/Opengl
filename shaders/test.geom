 #version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform mat3 normalMatrix;

in VS_OUT {
    vec2 texCoords;
    vec3 Normal;
    vec3 pos;
} gs_in[];

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;
out mat4 View;

uniform float time;


vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 10.0) * magnitude;
    return position + vec4(direction, 0.0);
}

vec3 GetNormal() {
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
 }

void main() {
   vec3 normal = GetNormal();
    for(int i =0;i<3;i++){
    vec4 newPos = explode(gl_in[i].gl_Position, normal);
        gl_Position = gl_in[i].gl_Position + newPos;
        FragPos = (view * model * vec4(gs_in[i].pos + normal / 4 , 1)).xyz;

        TexCoords = gs_in[i].texCoords;
        Normal = (view * vec4(normalMatrix * gs_in[i].Normal, 0)).xyz;

        View = view;
        EmitVertex();
    }

    EndPrimitive();
}