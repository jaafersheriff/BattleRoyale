#version 330 core

uniform sampler2D f_texCol;

in vec2 f_screenPos;

out vec4 color;

void main() {
    color = vec4(1.f, 1.f, 1.f, 1.f);
}