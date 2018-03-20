#version 330 core

in vec2 texCoords;

uniform sampler2D textureImage;
uniform bool usesTexture;

void main() {
    if(usesTexture && texture(textureImage, texCoords).a < 0.1) {
        discard;
    }
}

