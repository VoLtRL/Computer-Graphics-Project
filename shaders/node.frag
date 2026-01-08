#version 330 core

in vec3 fragColor;
in float visibility;

out vec4 out_color;

uniform vec4 fogColor; 

void main() {
    vec4 objectColor = vec4(fragColor, 1.0);

    out_color = mix(fogColor, objectColor, visibility);
}