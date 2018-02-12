#version 330 core

layout (location = 0) in vec3 in_pos;

uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;

void main() {
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_pos, 1.0f);
}