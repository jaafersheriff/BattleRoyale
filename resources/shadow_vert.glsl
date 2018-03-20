#version  330 core

const int MAX_PARTICLE_VARIATIONS = 20;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec2 vertTex;
layout(location = 2) in vec3 particlePosition;
layout(location = 3) in uint particleMatrixID;
layout(location = 4) in vec3 particleVelocity;
layout(location = 5) in float particleLife;

uniform mat4 L;
uniform mat4 M;
uniform bool particles;
uniform mat4 variationMs[MAX_PARTICLE_VARIATIONS];
uniform float particleScale;
uniform bool particleVariation;
uniform bool particleFade;
uniform float particleMaxLife;

out vec2 texCoords;

void main() {

    vec3 fragPos;

    if (particles) {
        float scale = particleScale;
        if (particleFade) {
            scale *= 1.0f - particleLife / particleMaxLife;
        }
        if (particleVariation) {
            fragPos = vec3(variationMs[particleMatrixID] * vec4(scale * vertPos, 1.0f)) + particlePosition;
        }
        else {
            fragPos = scale * vertPos + particlePosition;
        }
    }
	else {
		fragPos = vec3(M * vec4(vertPos, 1.0));
	}
    texCoords = vertTex;
    /* transform into light space */
    gl_Position = L * vec4(fragPos, 1.0f);

}