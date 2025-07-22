#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
    mat4 transform;
    mat4 normalMatrix;
} push;

const vec3 DIRECTIONAL_LIGHT = normalize(vec3(1, -3, 1));
const float AMBIENT_LIGHT = 0.05;

void main() {
    // 4d vector position
    gl_Position = push.transform * vec4(position, 1);

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
    float lightIntensity = AMBIENT_LIGHT + max(dot(normalWorldSpace, DIRECTIONAL_LIGHT), 0.0);
    fragColor = lightIntensity * color;
}