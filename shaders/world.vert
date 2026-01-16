#version 330 core

in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float fogStart;
uniform float fogEnd;

out vec3 fragPos;
out float visibility;

vec4 getCameraSpacePosition(vec3 pos) {
    return view * model * vec4(pos, 1.0);
}

vec4 getWorldSpacePosition(vec3 pos) {
    return model * vec4(pos, 1.0);
}

float getFogVisibility(float dist) {
    float vis = (fogEnd - dist) / (fogEnd - fogStart);
    return clamp(vis, 0.0, 1.0);
}

void main() {
    vec4 cameraSpacePos = getCameraSpacePosition(position);
    
    gl_Position = projection * cameraSpacePos;
    visibility = getFogVisibility(length(cameraSpacePos.xyz));
    fragPos = vec3(getWorldSpacePosition(position));
}