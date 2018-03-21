#version 330 core

layout(location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

uniform vec3 u_color;

void main() {
	color.rgb = u_color;
	color.a = 1.0f;

	BrightColor = vec4(0.0);
}