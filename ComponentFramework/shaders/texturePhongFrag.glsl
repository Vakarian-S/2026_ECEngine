#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 vertNormal;
layout(location = 1) in vec3 lightDir;
layout(location = 2) in vec3 eyeDir; 
layout(location = 3) in vec2 textureCoords;
layout(location = 4) in float lightDistance;


uniform sampler2D myTexture;
uniform vec4 ambientLightColor;
uniform vec4 diffuseMaterialColor;
uniform vec4 specularMaterialColor;
uniform float specularShininessExponent;
uniform float lightIntensityMultiplier;

void main() {
	vec4 textureSampleColor = texture(myTexture, textureCoords);

	vec3 normalizedSurfaceNormal = normalize(vertNormal);
	vec3 normalizedLightDirection = normalize(lightDir);
	vec3 normalizedEyeDirection = normalize(eyeDir);

	float diffuseFactor = max(dot(normalizedSurfaceNormal, normalizedLightDirection), 0.0);

	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source so flip the sign
	vec3 reflectionDirection = normalize(reflect(-normalizedLightDirection, normalizedSurfaceNormal));

	float specularFactor  = max(dot(normalizedEyeDirection, reflectionDirection), 0.0);
	specularFactor  = pow(specularFactor , specularShininessExponent);

	// simple physically-inspired attenuation (inverse square-ish)
	// add small constant so it doesn't explode when very close
	float attenuation = 1.0 / (1.0 + 0.09 * lightDistance + 0.032 * lightDistance * lightDistance);

	vec4 directLightComponent =
	attenuation * lightIntensityMultiplier *
	((diffuseFactor * diffuseMaterialColor) + (specularFactor * specularMaterialColor));

	fragColor = (ambientLightColor + directLightComponent) * textureSampleColor;
}