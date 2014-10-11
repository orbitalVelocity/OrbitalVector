#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
uniform float time;
out vec4 outColor;

void main(){
    vec4 color = texture( renderedTexture, UV );
    color.x = (color.x > 0.8f) ? color.x : 0.0f;
    color.y = (color.y > 0.8f) ? color.y : 0.0f;
    color.z = (color.z > 0.8f) ? color.z : 0.0f;
    outColor = color;
}
