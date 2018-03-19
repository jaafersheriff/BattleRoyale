#version 330 core

layout(location = 0) in vec2 v_vPos;

uniform vec2 v_scale;
uniform vec2 v_translate;
uniform float v_depth;

out vec2 f_screenPos;
out vec2 f_texPos;

void main() {
    vec2 screenPos;

    screenPos = vec2(v_vPos.x * v_scale.x, v_vPos.y * v_scale.y);
    screenPos = screenPos + v_translate;
    
    f_screenPos = (screenPos + vec2(1.f, 1.f)) / 2.f;
    gl_Position = vec4(screenPos.x, screenPos.y, v_depth, 1.f);

    f_texPos = (v_vPos + vec2(1.f, 1.f)) / 2.f;
}
