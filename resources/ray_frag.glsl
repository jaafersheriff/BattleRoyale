#version 330 core

in float v2f_progress;

out vec4 out_color;

const vec3 k_startColor = vec3(1.0f, 0.0f, 0.0f);
const vec3 k_endColor = vec3(0.0f, 1.0f, 1.0f);

void main() {
	out_color.rgb = mix(k_startColor, k_endColor, v2f_progress);
	out_color.a = 1.0f;
}