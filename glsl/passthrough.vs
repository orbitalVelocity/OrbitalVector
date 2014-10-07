#version 150 core
in vec3 position;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){
    gl_Position =  vec4(position,1.0);
    UV = (position.xy+vec2(1.0,1.0))/2.0;
}

