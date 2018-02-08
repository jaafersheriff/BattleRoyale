#version 330 core

in vec3 worldPos;
in vec3 fragNor;
in vec2 texCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;

uniform vec3 lightPos;

uniform sampler2D textureImage;
uniform bool usesTexture;

out vec4 color;

void main() {
    vec3 lightDir = lightPos - worldPos;
    vec3 L = normalize(lightDir);
    vec3 N = normalize(fragNor);

    /* Diffuse */
    float diffuseContrib = max(dot(L, N), matAmbient);
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, texCoords));
    }

    color = vec4(diffuseColor*diffuseContrib, 1.0f);
}
