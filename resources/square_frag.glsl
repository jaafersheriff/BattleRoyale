#version 330 core

uniform sampler2D f_texCol;

in vec2 f_screenPos;

out vec4 color;

void main() {
    // color = vec4(1.f, 1.f, 0.f, 1.f);
    color = texture(f_texCol, f_screenPos);
}