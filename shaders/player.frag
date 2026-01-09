#version 330 core

in float visibility;

out vec4 out_color;

uniform vec3 objectColor;
uniform vec4 fogColor; 

vec4 applyFog(vec4 baseColor, float vis) {
    return mix(fogColor, baseColor, vis);
}

void main() {
    out_color = applyFog(vec4(objectColor,1.0f), visibility);
}