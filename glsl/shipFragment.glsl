#version 150 core
out vec4 outColor;
//uniform vec3 color;
in vec3 fragNormal;
void main() {
    outColor = vec4(fragNormal, 1.0);
}