#version 330 core

layout (location = 0) in vec3 in_pos;

out float v2f_progress;

uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform float u_invLength;

void main() {
	v2f_progress = gl_VertexID * u_invLength;

	gl_Position = u_projMat * u_viewMat * vec4(in_pos, 1.0f);
}