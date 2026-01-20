#version 330 core

in vec3 fragPos;
in float visibility;

out vec4 out_color;

uniform vec3 objectColor;
uniform bool useCheckerboard;
uniform bool isEmissive;

uniform vec4 fogColor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightIntensity;

vec4 applyFog(vec4 baseColor, float vis) {
    return mix(fogColor, baseColor, vis);
}

vec3 getCheckerboardColor(vec3 position) {
    float f = floor(position.x) + floor(position.z);
    bool isEven = mod(f, 2.0) == 0.0;
    return isEven ? objectColor : objectColor * 0.5;
}

void main() {
    if (isEmissive) {
        out_color = applyFog(vec4(objectColor, 1.0), visibility);
        return;
    }

    vec3 baseColor;
    if (useCheckerboard) {
        baseColor = getCheckerboardColor(fragPos);
    } else {
        baseColor = objectColor;
    }

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.005 * distance * distance);
    
    vec3 lighting = lightColor * attenuation * lightIntensity;
    
    vec3 finalColorRGB = baseColor + lighting;

    out_color = applyFog(vec4(finalColorRGB, 1.0), visibility);
}