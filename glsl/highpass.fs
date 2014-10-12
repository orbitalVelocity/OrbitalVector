#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
uniform float time;
out vec4 outColor;

void main(){
    vec4 color = texture( renderedTexture, UV );
    float threshold = 1.0f;
    color.x = (color.x > threshold) ? color.x : 0.0f;
    color.y = (color.y > threshold) ? color.y : 0.0f;
    color.z = (color.z > threshold) ? color.z : 0.0f;
    outColor = color;
}
