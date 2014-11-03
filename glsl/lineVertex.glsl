#version 150 core
in vec3 position;
//out vec3 fragColor;
uniform mat4 model;
out float depth;
out gl_PerVertex {
    vec4 gl_Position;
};
void main() {
/*
    vec3 surfacePos = vec3(transform * vec4(position, 1.0));
     vec3 surfaceToLight = normalize(vec3(0.0, -10,10) - surfacePos);
     vec3 normal = normalize(transpose(inverse(mat3(transform))) * position);
     float diffuseCoefficient = max(0.0, dot(normal, -surfaceToLight));
 //*/
//   fragColor = position;//diffuseCoefficient * vec3(1.0,1.0,1.0);
   gl_Position = model * vec4(position, 1.0);
    depth = gl_Position.z;
}