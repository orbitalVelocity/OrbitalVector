#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
uniform float time;
out vec4 outColor;

void main(){
    outColor = texture( renderedTexture, UV + 0.005*vec2( sin(time+1024.0*UV.x),cos(time+768.0*UV.y)) ) ;
}
