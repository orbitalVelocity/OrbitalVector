#version 150 core
in vec3 fragNormal;
in vec3 fragVertex;
uniform vec3 lightPos;
uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 color;
out vec4 outColor;

void main() {
    vec3 light1 = vec3(0.3,0.5,0.4);
    vec3 light2 = vec3(-0.3,0.5,-0.4);
    vec3 green = vec3(0.0, 1.0, 0.0);
    vec3 specularColor = vec3(1, 1, 1);
    
    //specular lighting
    light2 = vec3(model * vec4(light2, 1.0));
    vec3 vertex = vec3(model * vec4(fragVertex, 1.0));
    vec3 _lightPos = vec3(model * vec4(lightPos, 1.0));
    vec3 incidentVector = vertex - _lightPos;
    vec3 reflectionVector = reflect(incidentVector, fragNormal);
    vec3 surfToCamera = normalize(vertex - cameraPos);
    float cosAngle = max(0.0, dot(surfToCamera, reflectionVector));
    float specularCoefficient = pow(cosAngle, 0.8);
    
    vec3 finalColor = color * max(0.0, dot(light2, fragNormal))
    + green * max(0.0, dot(light1, fragNormal))
                    + specularCoefficient * specularColor;
    outColor = vec4(finalColor, 1.0);
}