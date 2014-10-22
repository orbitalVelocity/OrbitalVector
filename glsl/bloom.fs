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
    float scale = 2;
    float offsetx = scale / width;//height;
    float offsety = scale / height;//width;
    int i;
    float offsetFactor = 2;
    float left = UV.x - offsetFactor * offsetx;
    float top = UV.y;// - offsetFactor * offsety;
    vec2 tc = vec2(left, top);
    scale = 2;

    vec4 c = texture(renderedTexture, UV);
    int passes = 8;
    c = vec4(0.0, 0.0, 0.0, 0.0);
    float lod = 2.0f;
    for (i = 0; i < passes; i++) {
        c += kernel[ 0] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += kernel[ 1] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += kernel[ 2] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += kernel[ 3] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += kernel[ 4] * textureLod(renderedTexture, tc, lod);
        //            tc.y += offsety;
        if (false) {
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
        }
        lod += 1.0f;
        left = UV.x - offsetFactor * offsetx;
        offsetx *= scale;
//        offsety *= scale;
//        top = UV.y - offsetFactor * offsety;
        tc = vec2(left, top);
    }
    c = c / 16.0f / float(passes);
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
