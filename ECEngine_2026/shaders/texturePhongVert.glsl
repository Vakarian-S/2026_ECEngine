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

layout(location = 0) out vec3 worldNormal;
layout(location = 1) out vec3 worldPosition;
layout(location = 2) out vec2 textureCoords;


void main() {
    textureCoords = uvCoord;
    textureCoords.y *= -1.0;

    mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
    worldNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 

    worldPosition = vec3(modelMatrix * vVertex);
    gl_Position = projectionMatrix * viewMatrix * vec4(worldPosition, 1.0);
}
