#version  330 core

layout(location = 0) in vec3 vertPos;

uniform mat4 L;
uniform mat4 M;

void main() {

  /* transform into light space */
  gl_Position = L * M * vec4(vertPos, 1.0);
}