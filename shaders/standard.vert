#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 anormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 fragPos;
out vec3 normal;
out vec4 fragPosLightSpace;

void main() {
    vec4 cameraSpacePos = view * model * vec4(position, 1.0);
    gl_Position = projection * cameraSpacePos;
    fragPos = vec3(model * vec4(position, 1.0));

    normal = normalize(normalMatrix * anormal);

    fragPosLightSpace = lightSpaceMatrix * vec4(position, 1.0);
}