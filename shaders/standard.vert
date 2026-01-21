#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float fogStart;
uniform float fogEnd;

out vec3 fragPos;
out float visibility;

void main() {
    vec4 cameraSpacePos = view * model * vec4(position, 1.0);
    gl_Position = projection * cameraSpacePos;

    fragPos = vec3(model * vec4(position, 1.0));

    float dist = length(cameraSpacePos.xyz);
    visibility = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
}