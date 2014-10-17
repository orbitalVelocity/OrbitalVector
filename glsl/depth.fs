#version 150 core

#if 0
out vec4 outColor;

void main(){
    vec3 color = vec3(.5);//gl_FragCoord.z/gl_FragCoord.w);
    outColor = vec4(color, 1.0);
}

#else

//out float outColor;

void main(){
    //    outColor = .5;//gl_FragCoord.z/gl_FragCoord.w;
//    gl_FragDepth = gl_FragCoord.z/gl_FragCoord.w;
}
#endif
