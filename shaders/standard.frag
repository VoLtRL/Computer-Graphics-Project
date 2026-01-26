#version 330 core

// Maximum number of point lights
#define MAX_LIGHTS 100

in vec3 fragPos; // fragment position in world space
in vec3 normal; // normal vector
in vec4 fragPosLightSpace; // fragment position in light space

out vec4 out_color; // final fragment color

// Uniforms
uniform vec3 objectColor; // base color of the object
uniform vec3 viewPos; // camera position
uniform bool useCheckerboard; // If true, apply checkerboard pattern
uniform bool isEmissive; // if true, object is emissive (it becomes a light source)

uniform vec4 fogColor; // color of the fog
uniform float fogStart; // distance where fog starts
uniform float fogEnd; // distance where fog ends

uniform int numActiveLights; // number of active point lights
uniform vec3 lightPos[MAX_LIGHTS]; // positions of point lights
uniform vec3 lightColors[MAX_LIGHTS]; // colors of point lights
uniform float lightIntensities[MAX_LIGHTS]; // intensities of point lights

uniform float alpha = 1.0; // alpha value for transparency

uniform sampler2D shadowMap; // shadow map texture
uniform vec3 dirLightPos; // directional light position (sun)
uniform bool isShadowPass; // if true, we are rendering the shadow pass

// Function to calculate shadow factor
float shadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) // outside the light's frustum
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005); // shadow bias
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; // get depth from shadow map
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }    
    }
    shadow /= 9.0; // average the shadow factor
    
    return shadow;
}

vec4 applyFog(vec4 baseColor, float vis) {
    return mix(fogColor, baseColor, vis); // linear interpolation between fog color and base color
}

// Checkerboard pattern function
vec3 getCheckerboardColor(vec3 position) {
    float f = floor(position.x) + floor(position.z);
    bool isEven = mod(f, 2.0) == 0.0;
    return isEven ? objectColor : objectColor * 0.5;
}

void main() {
    // Skip lighting calculations during shadow pass
    if (isShadowPass) {
        return;
    }

    vec3 viewPosXZ = vec3(viewPos.x, 0.0, viewPos.z); // camera position projected onto XZ plane
    vec3 fragPosXZ = vec3(fragPos.x, 0.0, fragPos.z); // fragment position projected onto XZ plane
    float horizontalDist = length(viewPosXZ - fragPosXZ); // horizontal distance from camera to fragment
    
    // fog visibility calculation
    float visibility = clamp((fogEnd - horizontalDist) / (fogEnd - fogStart), 0.0, 1.0);

    // Emissive objects are not affected by lighting
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

    // Lighting calculations (Phong model)
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    // ambient light
    vec3 ambient = 0.3 * baseColor;

    // Directional light (sun)
    vec3 lightDir = normalize(dirLightPos - fragPos);
    // diffuse light (Lambertian reflection)
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 sunColor = vec3(1.0, 0.95, 0.9); // warm sunlight color
    
    vec3 halfwayDir = normalize(lightDir + viewDir); // halfway vector (Blinn-Phong)
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * sunColor * 0.5; // specular highlight

    // Shadow calculation
    
    float shadow = shadowCalculation(fragPosLightSpace, norm, lightDir);
    vec3 sunLighting = (1.0 - shadow) * (diff * sunColor + specular);
    
    vec3 pointLightContribution = vec3(0.0); // initialize point light contribution
    // iterate over all active point lights to accumulate their contributions of lighting
    for(int i = 0; i < numActiveLights; i++) {
        vec3 plDir = normalize(lightPos[i] - fragPos);
        float plDiff = max(dot(norm, plDir), 0.0);
        
        float distance = length(lightPos[i] - fragPos);
        float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.005 * distance * distance);
        vec3 plHalfwayDir = normalize(plDir + viewDir);
        float plSpec = pow(max(dot(norm, plHalfwayDir), 0.0), 32.0);
        pointLightContribution += (plDiff * lightColors[i] + plSpec * lightColors[i]) * attenuation * lightIntensities[i];
    }
    
    // final color calculation with ambient, sun, and point light contributions
    vec3 finalColorRGB = ambient + (baseColor * sunLighting) + (baseColor * pointLightContribution);
    
    // apply fog to the final color
    out_color = applyFog(vec4(finalColorRGB, alpha), visibility);
}