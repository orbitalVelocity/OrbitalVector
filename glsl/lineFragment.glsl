#version 150 core
out vec4 outColor;
uniform vec3 color;
in float depth;
void main() {
   outColor = vec4(color, 1.0 - min(depth / 400.0f, 1.0f));
}