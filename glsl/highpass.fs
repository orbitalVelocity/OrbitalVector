#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
out vec4 outColor;

void main(){
    vec3 color = vec3(texture( renderedTexture, UV ).rgb);
    float threshold = 1.0f;
    color.x = (color.x > threshold) ? color.x : 0.0f;
    color.y = (color.y > threshold) ? color.y : 0.0f;
    color.z = (color.z > threshold) ? color.z : 0.0f;
    outColor = vec4(color, 1.0);
}
