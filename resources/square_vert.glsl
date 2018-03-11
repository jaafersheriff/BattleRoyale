#version 330 core

layout(location = 0) in vec2 v_screenPos;

out vec2 f_screenPos;

void main() {
    f_screenPos = (v_screenPos + vec2(1.f, 1.f)) / 2.f;
    gl_Position = vec4(v_screenPos, -1.f, 1.f);
}
