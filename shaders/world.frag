#version 330 core

in vec3 fragPos;
in float visibility;

out vec4 out_color;

uniform vec3 objectColor;
uniform vec4 fogColor; 

vec4 applyFog(vec4 baseColor, float vis) {
    return mix(fogColor, baseColor, vis);
}

vec3 getColorFromPosition(vec3 position) {
    float f = floor(position.x) + floor(position.z);
    
    bool isEven = mod(f, 2.0) == 0.0;
    vec3 finalColor;

    if (isEven) {
        finalColor = objectColor;
    } else {
        finalColor = objectColor * 0.5;
    }
    return finalColor;
}
void main() {
    out_color = applyFog(vec4(getColorFromPosition(fragPos),1), visibility);
}