#version 330 core

uniform sampler2D f_texCol;

in vec2 f_screenPos;
in vec2 f_texPos;

out vec4 color;

void main() {
    // color = texture(f_texCol, f_screenPos);
    color = texture(f_texCol, f_texPos);
    // color = vec4(1.0, 1.0, 0.0, 1.0);
}