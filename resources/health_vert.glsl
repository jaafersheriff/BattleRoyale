#version 330

layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;

uniform vec3 center;
uniform vec2 size;

out vec2 texCoords;
out vec3 worldPos;

void main() {
    vec3 cameraRight = vec3(V[0][0], V[1][0], V[2][0]);
    vec3 cameraUp = vec3(V[0][1], V[1][1], V[2][1]);
    worldPos = 
        center
        + cameraRight * vertPos.x * size.x
        + cameraUp * vertPos.y * size.y;
    gl_Position = P * V * vec4(worldPos, 1.0);
    texCoords = (vertPos.xy + 1.0) / 2.0;
}