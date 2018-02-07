#version 330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 verTex;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform vec3 cameraPos;

out vec4 worldPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {
    worldPos = M * vertPos;
    gl_Position = P * V * worldPos;

    fragNor = vec3(M * vec4(vertNor, 0.0));
    texCoords = vertTex;
}