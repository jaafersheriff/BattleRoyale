#version 330 core

in vec4 worldPos;
in vec3 fragNor;
in vec2 texCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;

uniform vec3 lightPos;

uniform sampler2D textureImage;
uniform bool usesTexture;

out vec4 color;

void main() {
    vec3 lightDir = lightPos - worldPos.xyz;
    vec3 L = normalize(lightDir);
    vec3 N = normalize(fragNor);

    float lightDistance = length(lightDir);

    /* Diffuse */
    float diffuseContrib = max(dot(L, N), matAmbient);
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, texCoords));
    }

    color = vec4(fragNor.xyz, 1.0); //diffuseColor*diffuseContrib, 1);
}