#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 L;
uniform mat4 M;
uniform mat3 N;
uniform vec2 tiling;

uniform vec3 cameraPos;

uniform float lightDist;
uniform float transitionDistance;

out vec3 fragPos;
out vec4 fragLPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {
    fragPos = vec3(M * vec4(vertPos, 1.0f));
    fragLPos = L * vec4(fragPos, 1.0);
    vec4 fragVPos = V * vec4(fragPos, 1.0);
    fragNor = N * vertNor;
    texCoords = vec2(vertTex.x*tiling.x, vertTex.y*tiling.y);

    gl_Position = P * fragVPos;

    float dist = (length(fragVPos.xyz) - lightDist - transitionDistance) / transitionDistance;
    fragLPos.w = clamp(1 - dist, 0.0, 1.0);
}