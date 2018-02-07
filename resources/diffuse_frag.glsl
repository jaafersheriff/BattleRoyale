#version 330 core

in vec4 worldPos;
in vec3 fragNormal;
in vec2 textureCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float shine;

uniform vec3 lightPos;

uniform sampler2D textureImage;
uniform bool usesTexture;

out vec4 color;

void main() {
    vec3 lightDir = lightPos - worldPos.xyz;
    vec3 L = normalize(lightDir);
    vec3 N = normalize(fragNormal);

    float lightDistance = length(lightDir);

    /* Diffuse */
    vec3 diffuseContrib = max(dot(L, N), matAmbient) / attFactor;
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, textureCoords));
    }

    color = vec4(diffuseColor*diffuseContrib, 1);
}