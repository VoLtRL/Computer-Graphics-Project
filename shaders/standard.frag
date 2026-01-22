#version 330 core

#define MAX_LIGHTS 100

in vec3 fragPos;
in float visibility;
in vec3 normal;
in vec4 fragPosLightSpace;

out vec4 out_color;

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform bool useCheckerboard;
uniform bool isEmissive;

uniform vec4 fogColor;
uniform int numActiveLights;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform float lightIntensities[MAX_LIGHTS];

uniform sampler2D shadowMap;
uniform vec3 dirLightPos;
uniform bool isShadowPass;

float shadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

vec4 applyFog(vec4 baseColor, float vis) {
    return mix(fogColor, baseColor, vis);
}

vec3 getCheckerboardColor(vec3 position) {
    float f = floor(position.x) + floor(position.z);
    bool isEven = mod(f, 2.0) == 0.0;
    return isEven ? objectColor : objectColor * 0.5;
}

void main() {
    if (isShadowPass) {
        return;
    }

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

    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 ambient = 0.3 * baseColor;

    vec3 lightDir = normalize(dirLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 sunColor = vec3(1.0, 0.95, 0.9);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * sunColor * 0.5;
    
    float shadow = shadowCalculation(fragPosLightSpace, norm, lightDir);

    vec3 sunLighting = (1.0 - shadow) * (diff * sunColor + specular);
    
    vec3 pointLightContribution = vec3(0.0);
    for(int i = 0; i < numActiveLights; i++) {
        vec3 plDir = normalize(lightPos[i] - fragPos);
        float plDiff = max(dot(norm, plDir), 0.0);
        
        float distance = length(lightPos[i] - fragPos);
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.005 * distance * distance);

        vec3 plHalfwayDir = normalize(plDir + viewDir);
        float plSpec = pow(max(dot(norm, plHalfwayDir), 0.0), 32.0);
    
        pointLightContribution += (plDiff * lightColors[i] + plSpec * lightColors[i]) * attenuation * lightIntensities[i];
    }
    
    vec3 finalColorRGB = ambient + (baseColor * sunLighting) + (baseColor * pointLightContribution);
    
    out_color = applyFog(vec4(finalColorRGB, 1.0), visibility);
}