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

//uniform sampler2D shadowMap;
uniform sampler2DShadow shadowMap;

void main() {
    float intensity = 1.3f;
    vec3 _lightPos = vec3(world * vec4(lightPos, 1.0));
    vec3 _cameraPos = cameraPos;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 normal = normalize(normalMatrix * fragNormal);
    vec3 surfacePos = vec3( world * model * vec4(fragVertex, 1.0));
    vec3 surfToLight = normalize(_lightPos - surfacePos);
    
    //specular lighting
    vec3 lightPos2 = vec3(0,1000,0);
    lightPos2 = vec3(vec4(lightPos2, 1.0));
    vec3 specularColor = vec3(3, 2.8, 2.7);
    
    vec3 incidentVector = -normalize(lightPos2 - surfacePos);
    vec3 reflectionVector = reflect(incidentVector, normal);
    vec3 surfToCamera = normalize(_cameraPos - surfacePos);
    float cosAngle = max(0.0, dot(surfToCamera, reflectionVector));
    float specularCoefficient = pow(cosAngle, 12);
    
    float defuse = max(0.0, dot(normal, surfToLight) * intensity);
    vec3 finalColor = color * defuse
//    ;
     + specularCoefficient * specularColor;

//    vec3 P =vec3(shadowCoord.x, shadowCoord.y, shadowCoord.z);///shadowCoord.w);
    float visibility = 1.0;
//    vec3 P = vec3(shadowCoord.xy, shadowCoord.z / shadowCoord.w);
    vec3 P = shadowCoord.xyz;// / shadowCoord.w;
    visibility = texture( shadowMap, P);
//    vec3 textureCoord = shadowCoord.xyz/ shadowCoord.w;
//    if (texture(shadowMap, textureCoord.xy).x < textureCoord.z)
//    if (texture(shadowMap, shadowCoord.xy).z ==0)// shadowCoord.z)
//    if (shadowCoord.z == 0)
//        visibility = 0.06;
    
    finalColor *= visibility;
//    finalColor = vec3(visibility);
    outColor = vec4(finalColor, 1.0);
}