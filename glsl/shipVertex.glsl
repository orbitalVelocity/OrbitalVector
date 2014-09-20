#version 150 core
in vec3 position;
in vec3 normal;
//in vec3 vertices;
out vec3 fragNormal;
//out vec3 fragVertices;
//out vec3 fragColor;
uniform mat4 transform;
void main() {
    /*
     vec3 surfacePos = vec3(transform * vec4(position, 1.0));
     vec3 surfaceToLight = normalize(vec3(0.0, -10,10) - surfacePos);
     vec3 normal = normalize(transpose(inverse(mat3(transform))) * position);
     float diffuseCoefficient = max(0.0, dot(normal, -surfaceToLight));
     //*/
    //   fragColor = position;//diffuseCoefficient * vec3(1.0,1.0,1.0);
    fragNormal = normal;
    gl_Position = transform * vec4(position, 1.0);
}