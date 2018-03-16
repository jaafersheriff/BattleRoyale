#version 330 core

const int MAX_VARIATIONS = 20;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 particlePosition;
layout(location = 4) in uint particleMatrixID;
layout(location = 5) in vec3 particleVelocity;
layout(location = 6) in float particleLife;

uniform mat4 P;
uniform mat4 V;
uniform mat4 variationMs[MAX_VARIATIONS];
uniform mat3 variationNs[MAX_VARIATIONS];
uniform float particleScale;
uniform bool particleVariation;
uniform bool particleFade;
uniform float particleMaxLife;

out vec3 worldPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {
    float scale = particleScale;
    if (particleFade) {
        scale *= 1.0f - particleLife / particleMaxLife;
    }

    if (particleVariation) {
        worldPos = vec3(variationMs[particleMatrixID] * vec4(scale * vertPos, 1.0f)) + particlePosition;
        fragNor = variationNs[particleMatrixID] * vertNor;
    }
    else {
        worldPos = scale * vertPos + particlePosition;
        fragNor = vertNor;
    }
    
    texCoords = vertTex;
    gl_Position = P * V * vec4(worldPos, 1.0f);    
}
