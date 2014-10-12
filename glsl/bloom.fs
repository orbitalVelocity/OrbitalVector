#version 150 core

in vec2 UV;

uniform sampler2D renderedTexture;
uniform sampler2D forwardTexture;
uniform float kernel[25];
uniform vec2 frameSize;
out vec4 outColor;

//adapted from http://prideout.net/archive/bloom/
void main(){
    float width = frameSize.x;
    float height = frameSize.y;
    float offsetx = 1.0f / height;//width;
    float offsety = 1.0f / width ;
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    int i;
    float left = UV.s - 2 * offsetx;
    float top = UV.t - 2 * offsety;
    vec2 tc = vec2(left, top);
    
    //    for (i = 0; i < 25; i++) {
    //        color += kernel[i] * textureLod(renderedTexture, UV + vec2(1/width*offsetx, 1/height*offsety)) / 256.0f;
    //    }
    c = texture(renderedTexture, UV);
    if (true){//c.r + c.g + c.b > 1.0f) {
        c = vec4(0.0, 0.0, 0.0, 0.0);
        float lod = 0.0f;
        for (i = 0; i < 8; i++) {
            c += kernel[ 0] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 1] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 2] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 3] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 4] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
            tc.x = left;
            c += kernel[ 5] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 6] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 7] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 8] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[ 9] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
            tc.x = left;
            c += kernel[10] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[11] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[12] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[13] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[14] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
            tc.x = left;
            c += kernel[15] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[16] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[17] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[18] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[19] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
            tc.x = left;
            c += kernel[20] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[21] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[22] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[23] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
            c += kernel[24] * textureLod(renderedTexture, tc, lod);
            lod += 1.0f;
            offsety *= 2.0f;
            offsetx *= 2.0f;
            left = UV.s - 2 * offsetx;
            top = UV.t - 2 * offsety;
            tc = vec2(left, top);
        }
        c = c / 256.0f / 8.0f;
    }
    c += texture(forwardTexture, UV);
    
    //tone mapping
    //Jim Hejl and Richard Burgess-Dawson
//    vec3 x;
//    x.r= max(0,c.r-0.004);
//    x.g= max(0,c.g-0.004);
//    x.b= max(0,c.b-0.004);
//    c.rgb = (x*(6.2*x+.5))/(x*(6.2*x+1.7)+0.06);
    //Reinhard
//    c = c / (1.0 + c);
//    c.x = pow(c.x, 1.0/2.2);
//    c.y = pow(c.y, 1.0/2.2);
//    c.z = pow(c.z, 1.0/2.2);
    outColor = c;
}
