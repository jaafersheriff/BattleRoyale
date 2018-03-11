#version 330 core

/* do nothing - pass through as depth written to FBO */

in vec4 wPos;

out vec4 color;

void main() {
    color = vec4(normalize(wPos.xyz), 1.0);
}

