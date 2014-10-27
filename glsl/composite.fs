#version 150 core

in vec2 UV;
uniform sampler2D tex0, tex1, tex2;
uniform float enable[3];
out vec4 outColor;
void main()
{
    outColor = enable[0] * texture(tex0, UV)
    + texture(tex1, UV) * enable[1]
    + texture(tex2, UV) * enable[2];
}