#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
out vec4 outColor;

void main(){
    outColor = texture( renderedTexture, UV );
}
