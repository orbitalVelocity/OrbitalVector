#version 150 core
in vec3 position;
uniform mat4 model;
uniform mat4 world;
uniform mat4 camera;
out gl_PerVertex {
    vec4 gl_Position;
};
void main(){
    gl_Position = model * vec4(position,1.0);
}

