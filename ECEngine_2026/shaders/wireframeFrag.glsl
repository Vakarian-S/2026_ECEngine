#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

// Wireframe color uniform: set per-component type (green for sphere, yellow for AABB)
layout(location = 3) uniform vec4 wireColor;

void main() {
    fragColor = wireColor;
}

