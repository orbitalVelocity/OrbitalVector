#version 150 core
in vec3 position;
uniform mat4 model;
uniform vec3 centralPos;
out gl_PerVertex {
    vec4 gl_Position;
};
void main() {
    
    gl_Position = model * vec4(centralPos, 1.0);
    gl_Position /= gl_Position.w;
    gl_Position.xy += position.xy * vec2(.1, .1);
}