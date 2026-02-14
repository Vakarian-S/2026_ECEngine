#version 450
#extension GL_ARB_separate_shader_objects: enable

const int maximumPointLightCount = 8;
uniform int activePointLightCount;

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec3 worldNormal;
layout (location = 1) in vec3 worldPosition;
layout (location = 2) in vec2 textureCoords;

uniform sampler2D myTexture;
uniform vec3 pointLightWorldPosition;
uniform vec3 cameraWorldPosition;
uniform vec4 ambientLightColor;
uniform vec4 diffuseMaterialColor;
uniform vec4 specularMaterialColor;
uniform float specularShininessExponent;
uniform float lightIntensityMultiplier;
uniform float attenuationConstant;
uniform float attenuationLinear;
uniform float attenuationQuadratic;


uniform vec3 pointLightWorldPositionArray[maximumPointLightCount];
uniform vec3 pointLightDiffuseLightColorArray[maximumPointLightCount];
uniform vec3 pointLightSpecularLightColorArray[maximumPointLightCount];

uniform float pointLightIntensityMultiplierArray[maximumPointLightCount];
uniform float pointLightAttenuationConstantArray[maximumPointLightCount];
uniform float pointLightAttenuationLinearArray[maximumPointLightCount];
uniform float pointLightAttenuationQuadraticArray[maximumPointLightCount];

void main() {
    vec4 textureSampleColor = texture(myTexture, textureCoords);

    // Camera and World
    vec3 normalizedSurfaceNormal = normalize(worldNormal);
    vec3 normalizedEyeDirection = normalize(cameraWorldPosition - worldPosition);

    // Light in our WorldSpace
    vec3 lightVectorWorldSpace = pointLightWorldPosition - worldPosition;
    float lightDistance = length(lightVectorWorldSpace);
    vec3 normalizedLightDirection = lightVectorWorldSpace / max(lightDistance, 0.0001);

    float diffuseFactor = max(dot(normalizedSurfaceNormal, normalizedLightDirection), 0.0);

    /// Reflection is based incedent which means a vector from the light source
    /// not the direction to the light source so flip the sign
    vec3 reflectionDirection = normalize(reflect(-normalizedLightDirection, normalizedSurfaceNormal));

    float specularFactor = max(dot(normalizedEyeDirection, reflectionDirection), 0.0);
    specularFactor = pow(specularFactor, specularShininessExponent);

    // simple physically-inspired attenuation (inverse square-ish)
    // add small constant so it doesn't explode when very close
    float attenuation = 1.0 / (attenuationConstant + attenuationLinear * lightDistance + attenuationQuadratic * lightDistance * lightDistance);


    // Multiple Lights
    vec3 accumulatedDiffuseLightRgb = vec3(0.0);
    vec3 accumulatedSpecularLightRgb = vec3(0.0);
    // Iterate Through Lights
    for (int pointLightIndex = 0; pointLightIndex < activePointLightCount; pointLightIndex++)
    {
        // Light in our WorldSpace
        vec3 lightVectorWorldSpace =
        pointLightWorldPositionArray[pointLightIndex] - worldPosition;
        float lightDistance = length(lightVectorWorldSpace);
        vec3 normalizedLightDirection =
        lightVectorWorldSpace / max(lightDistance, 0.0001);

        // Attentiation
        float attenuationFactor = 1.0 /
        (pointLightAttenuationConstantArray[pointLightIndex] +
        pointLightAttenuationLinearArray[pointLightIndex] * lightDistance +
        pointLightAttenuationQuadraticArray[pointLightIndex] * lightDistance * lightDistance);

        float diffuseFactor = max(dot(normalizedSurfaceNormal, normalizedLightDirection), 0.0);
        vec3 reflectionDirection = reflect(-normalizedLightDirection, normalizedSurfaceNormal);

        float specularFactor = max(dot(normalizedEyeDirection, normalize(reflectionDirection)), 0.0);
        specularFactor = pow(specularFactor, specularShininessExponent);

        float scaledLightIntensity =
        attenuationFactor * pointLightIntensityMultiplierArray[pointLightIndex];

        accumulatedDiffuseLightRgb +=
        scaledLightIntensity * diffuseFactor * pointLightDiffuseLightColorArray[pointLightIndex];

        accumulatedSpecularLightRgb +=
        scaledLightIntensity * specularFactor * pointLightSpecularLightColorArray[pointLightIndex];

    }

    vec3 finalLightingRgb =
    ambientLightColor.rgb +
    (accumulatedDiffuseLightRgb) +
    (accumulatedSpecularLightRgb);

    fragColor = vec4(finalLightingRgb, 1.0) * textureSampleColor;
    
}