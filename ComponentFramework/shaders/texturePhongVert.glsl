#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPos;
uniform vec3 cameraPos;

layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir;
layout(location = 2) out vec3 eyeDir;
layout(location = 3) out vec2 textureCoords;
layout(location = 4) out float lightDistance;


void main() {
    textureCoords = uvCoord;
    textureCoords.y *= -1.0;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 

    // world-space position for lighting
    vec3 worldPos = vec3(modelMatrix * vVertex);

    // light direction in world space
    vec3 lightVector = lightPos - worldPos;
    lightDistance = length(lightVector);
    lightDir = normalize(lightVector);

    // eye direction: easiest is compute camera position in world space
    // if you don't have cameraPos uniform yet, see option below
    // (for now, keep your existing eyeDir logic if you want zero extra uniforms)

    // keep your current approach (but it is view-space based, so it won't match world-space perfectly)
    vec3 vertPosView = vec3(viewMatrix * vec4(worldPos, 1.0));
    vec3 vertDir = normalize(vertPosView);
    eyeDir = normalize(cameraPos - worldPos);
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vVertex;
    
}
