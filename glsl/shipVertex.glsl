#version 150 core
in vec3 position;
in vec3 normal;
out vec3 fragNormal;
out vec3 fragVertex;
out vec4 shadowCoord;
uniform mat4 model;
uniform mat4 world;
uniform mat4 camera;
uniform mat4 DepthBiasMVP;

void main() {

    fragNormal = normal;
    fragVertex = position;
    gl_Position = camera * model * vec4(position, 1.0);
    shadowCoord = DepthBiasMVP * vec4(position, 1.0);
}