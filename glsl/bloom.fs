#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
uniform float kernel[25];
out vec4 outColor;

void main(){
    float width = 1280.0f;
    float height = 720.0f;
    float offsetx = 1.0f / width;
    float offsety = 1.0f / height;
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    int i;
    float left = UV.s - 2 * offsetx;
    float top = UV.t - 2 * offsety;
    vec2 tc = vec2(left, top);
    
//    for (i = 0; i < 25; i++) {
//        color += kernel[i] * texture(renderedTexture, UV + vec2(1/width*offsetx, 1/height*offsety)) / 256.0f;
//    }
    
    c += kernel[ 0] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 1] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 2] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 3] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 4] * texture(renderedTexture, tc); tc.y += offsety;
    tc.x = left;
    c += kernel[ 5] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 6] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 7] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 8] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[ 9] * texture(renderedTexture, tc); tc.y += offsety;
    tc.x = left;
    c += kernel[10] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[11] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[12] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[13] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[14] * texture(renderedTexture, tc); tc.y += offsety;
    tc.x = left;
    c += kernel[15] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[16] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[17] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[18] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[19] * texture(renderedTexture, tc); tc.y += offsety;
    tc.x = left;
    c += kernel[20] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[21] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[22] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[23] * texture(renderedTexture, tc); tc.x += offsetx;
    c += kernel[24] * texture(renderedTexture, tc);
    
    c = c / 256.0f;
    outColor = c;
}
