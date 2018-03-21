#version 330 core

in float v2f_progress;

layout(location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

const vec3 k_startColor = vec3(1.0f, 0.0f, 0.0f);
const vec3 k_endColor = vec3(0.0f, 1.0f, 1.0f);

void main() {
	color.rgb = mix(k_startColor, k_endColor, v2f_progress);
	color.a = 1.0f;

	BrightColor = vec4(0.0);
}