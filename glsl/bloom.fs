#version 150 core
//adapted from http://prideout.net/archive/bloom/

in vec2 UV;

uniform sampler2D renderedTexture;
uniform sampler2D forwardTexture;
out vec4 outColor;
#if 1
uniform vec2 offset;
uniform float weight[3] = float[](0.2270270270*2.0, 0.3162162162, 0.0702702703 );
void main(){
    //float coefficients[3] = {5.0, 6.0, 5.0};
    vec4 c = vec4(0,0,0,0);
    vec2 tc = UV;
    vec2 offset2 = offset;
    int i, passes = 8;
//    float lod = float(passes-1);
    float lod = 0;
    float reduce = 2.0f;
    for (i = 0; i < passes; i++)
    {
        c += weight[2] * textureLod(renderedTexture, tc - offset2 - offset2, lod);
        c += weight[1] * textureLod(renderedTexture, tc - offset2, lod);
        c += weight[0] * textureLod(renderedTexture, tc,          lod);
        c += weight[1] * textureLod(renderedTexture, tc + offset2, lod);
        c += weight[2] * textureLod(renderedTexture, tc + offset2 + offset2, lod);
        lod += 1.0f;
        offset2 = offset2 * 2.0;
//        lod-=1.0f;
//        offset2 = offset2 / 2.0;
//        c /= reduce;
//        reduce -= 1/float(passes-1);
    }
    c = c / float(passes);
    c += texture(forwardTexture, tc);
    outColor = c;
}
#else
uniform vec2 frameSize;
//uniform float kernel[25];
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

    int passes = 8;
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    float lod = 2.0f;
    for (i = 0; i < passes; i++) {
        c += 1.0f * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += 4.0f * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += 6.0f * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += 4.0f * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
        c += 1.0f * textureLod(renderedTexture, tc, lod);
        //            tc.y += offsety;
        lod += 1.0f;
        offsetx *= scale;
        left = UV.x - offsetFactor * offsetx;
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

//if (false) {
//    tc.x = left;
//    c += kernel[ 5] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[ 6] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[ 7] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[ 8] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[ 9] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
//    tc.x = left;
//    c += kernel[10] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[11] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[12] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[13] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[14] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
//    tc.x = left;
//    c += kernel[15] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[16] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[17] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[18] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[19] * textureLod(renderedTexture, tc, lod); tc.y += offsety;
//    tc.x = left;
//    c += kernel[20] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[21] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[22] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[23] * textureLod(renderedTexture, tc, lod); tc.x += offsetx;
//    c += kernel[24] * textureLod(renderedTexture, tc, lod);
//}

#endif
