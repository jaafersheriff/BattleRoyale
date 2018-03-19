#version 330 core

uniform sampler2D f_texCol;

in vec2 f_texPos;

out vec4 color;

void main() {
    color = texture(f_texCol, f_texPos);
}