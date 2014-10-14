#version 150 core
in vec3 fragNormal;
in vec3 fragVertex;
in vec4 shadowCoord;
uniform vec3 lightPos;
uniform mat4 model;
uniform mat4 world;
uniform vec3 cameraPos;
uniform vec3 color;
out vec4 outColor;

uniform sampler2D shadowMap;
//uniform sampler2DShadow shadowMap;

void main() {
    vec3 light2 = vec3(0.3,0.5,0.4);
    vec3 specularColor = vec3(0, 1, 1);
    float intensity = 3.0f;
    //specular lighting
//    vec3 _cameraPos= vec3(world * vec4(cameraPos, 1.0));
//    vec3 normal = normalize(transpose(inverse(mat3(model)))*fragNormal);
//    vec3 normal = fragNormal;
    light2 = vec3(world * vec4(light2, 1.0));
    vec3 _lightPos = vec3(world * vec4(lightPos, 1.0));
    vec3 _cameraPos = cameraPos;
    vec3 normal = normalize(vec3(model * vec4(fragNormal, 1.0)));
    vec3 surfacePos = vec3( model * vec4(fragVertex, 1.0));
    vec3 surfToLight = normalize(_lightPos - surfacePos);
    
    vec3 incidentVector = surfToLight;//normalize(_lightPos - surfacePos);
    vec3 reflectionVector = reflect(incidentVector, normal);
    vec3 surfToCamera = normalize(_cameraPos - surfacePos);
    float cosAngle = max(0.0, dot(surfToCamera, reflectionVector));
    float specularCoefficient = pow(cosAngle, 12);
    
    vec3 finalColor = color * max(0.0, dot(normal, -surfToLight) * intensity)
    ;// + specularCoefficient * specularColor;
//    finalColor = cameraPos;
//    finalColor.xyz = vec3(gl_FragCoord.w);
    float visibility = 1.0f;
    if ( texture(shadowMap, shadowCoord.xy).z < shadowCoord.z)
        visibility = 0.5;
    finalColor = vec3(visibility);//visibility * finalColor;
//    finalColor = visibiity * finalColor;
    outColor = vec4(finalColor, 1.0);
}