#version 150 core
//adapted from http://prideout.net/archive/bloom/

in vec2 UV;

uniform sampler2D renderedTexture;
uniform sampler2D forwardTexture;
out vec4 outColor;

uniform vec2 offset;
//uniform float weight[3] = float[](0.2270270270*1.0, 0.3162162162, 0.0702702703 );
uniform float weight[5] = float[](0.16, 0.15, 0.12, 0.09, 0.05);
void main(){
    float offset1 = 1.3846153846;
    float offset2 = 3.2307692308;
    
    vec4 c = vec4(0,0,0,0);
    vec2 tc = UV;
    vec2 offset0 = offset * 4;//vec2(1.2/1920.0, 0);
    vec2 bias;
    bias.x = 0/1920.0;
    vec2 useOffset1 = offset0 * 1.0;//offset1;
    vec2 useOffset2 = offset0 * 2.0;//offset2;
    vec2 useOffset3 = offset0 * 3.0;
    vec2 useOffset4 = offset0 * 4.0;
    int i, passes = 2;
    //    float lod = float(passes-1);
    float lod = 2;
    float reduce = 1.0f;
    for (i = 0; i < passes; i++)
    {
        //        c += weight[4] * textureLod(renderedTexture, tc - useOffset4, lod);
        //        c += weight[3] * textureLod(renderedTexture, tc - useOffset3, lod);
        c += weight[2] * textureLod(renderedTexture, tc - useOffset2 - bias, lod);
        c += weight[1] * textureLod(renderedTexture, tc - useOffset1 - bias, lod);
        c += weight[0] * textureLod(renderedTexture, tc - bias,              lod);
        c += weight[1] * textureLod(renderedTexture, tc + useOffset1 - bias, lod);
        c += weight[2] * textureLod(renderedTexture, tc + useOffset2 - bias, lod);
        //        c += weight[3] * textureLod(renderedTexture, tc + useOffset3, lod);
        //        c += weight[4] * textureLod(renderedTexture, tc + useOffset4, lod);
        lod += 1.0f;
        useOffset1 *= 2.0;
        useOffset2 *= 2.0;
        
    }
    c = c / float(passes)/1.8;// /2.0;
    //    c = textureLod(renderedTexture, tc,          lod);
    //    if (offset.y != 0.0)
    c += texture(forwardTexture, tc);
    outColor = c;
}

