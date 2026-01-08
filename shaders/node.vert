#version 330 core

// input attribute variable, given per vertex
in vec3 position;

// global matrix variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float fogStart;
uniform float fogEnd;

out vec3 fragColor;
out float visibility;

void main() {
    fragColor = position * vec3(2,2,2);

    vec4 relativeToCam = view * model * vec4(position, 1.0);
    
    float distance = length(relativeToCam.xyz);

    visibility = (fogEnd - distance) / (fogEnd - fogStart);
    visibility = clamp(visibility, 0.0, 1.0);

    gl_Position = projection * relativeToCam;
}