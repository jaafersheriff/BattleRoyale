#version 330 core

const int MAX_PARTICLE_VARIATIONS = 20;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 particlePosition;
layout(location = 4) in uint particleMatrixID;
layout(location = 5) in vec3 particleVelocity;
layout(location = 6) in float particleLife;

uniform mat4 P;
uniform mat4 V;
uniform mat4 L;
uniform mat4 M;
uniform mat3 N;
uniform vec2 tiling;
uniform bool particles;
uniform mat4 variationMs[MAX_PARTICLE_VARIATIONS];
uniform mat3 variationNs[MAX_PARTICLE_VARIATIONS];
uniform float particleScale;
uniform bool particleVariation;
uniform bool particleFade;
uniform float particleMaxLife;

uniform float lightDist;
uniform float transitionDistance;

out vec3 fragPos;
out vec4 fragLPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {

    if (particles) {

        float scale = particleScale;
        if (particleFade) {
            scale *= 1.0f - particleLife / particleMaxLife;
        }

        if (particleVariation) {
            fragPos = vec3(variationMs[particleMatrixID] * vec4(scale * vertPos, 1.0f)) + particlePosition;
            fragNor = variationNs[particleMatrixID] * vertNor;
        }
        else {
            fragPos = scale * vertPos + particlePosition;
            fragNor = vertNor;
        }

    }
    else {

        fragPos = vec3(M * vec4(vertPos, 1.0f));
        fragLPos = L * vec4(fragPos, 1.0);
        fragNor = N * vertNor;

    }

    vec4 fragVPos = V * vec4(fragPos, 1.0);
    texCoords = vec2(vertTex.x*tiling.x, vertTex.y*tiling.y);

    gl_Position = P * fragVPos;

    float dist = (length(fragVPos.xyz) - lightDist - transitionDistance) / transitionDistance;
    fragLPos.w = clamp(1 - dist, 0.0, 1.0);
}