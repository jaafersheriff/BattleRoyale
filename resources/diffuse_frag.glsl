#version 330 core

in vec3 worldPos;
in vec3 fragNor;
in vec2 texCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;

uniform vec3 camPos;
uniform vec3 lightPos;

uniform sampler2D textureImage;
uniform bool usesTexture;

uniform bool isToon;
uniform float silAngle;
uniform float cells;

out vec4 color;

void main() {
    vec3 lightDir = lightPos - worldPos;
    vec3 viewDir = camPos - worldPos;
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);
    vec3 N = normalize(fragNor);

    /* Diffuse */
    float diffuseContrib = clamp(dot(L, N), matAmbient, 1.0);
    /* Cell shading */
    if (isToon) {
        float level = floor(diffuseContrib * cells);
        diffuseContrib = level / cells;
    }
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, texCoords));
    }

    color = vec4(diffuseColor*diffuseContrib, 1.0f);

    /* Silhouettes */
    if (isToon) {
        float angle = dot(N, V);
        if(angle > 0 && angle < silAngle) {
            color = vec4(0, 0, 0, 1);
        }
    }
}
