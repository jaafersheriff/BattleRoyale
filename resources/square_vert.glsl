#version 330 core

layout(location = 0) in vec2 screenPos;

void main() {
    gl_Position = screenPos;
}
