#version 330 core

layout(location = 0) in vec2 v_vPos;

uniform int v_operation;

out vec2 f_texPos;
flat out int f_operation;

void main() {
    f_operation = v_operation;
    //f_operation = 1;
    f_texPos = (v_vPos + vec2(1.f, 1.f)) / 2.f;
    gl_Position = vec4(v_vPos, -1.f, 1.f);
}
