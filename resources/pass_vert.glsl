#version  330 core
layout(location = 0) in vec2 v_screenPos;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(v_screenPos, -1.f, 1.f);
    TexCoords = (v_screenPos.xy+vec2(1.f, 1.f))/2.f;
}
