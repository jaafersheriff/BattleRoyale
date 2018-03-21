#version 330

in vec2 texCoords;
in vec3 worldPos;

uniform float minVal;
uniform float curVal;
uniform float maxVal;

out vec4 color;

void main() {
    float perc = (curVal - minVal) / (maxVal - minVal);
    if (perc > texCoords.x) {
        color = vec4(0, 1, 0, 1);
    }
    else {
        color = vec4(1, 0, 0, 1);
    }
}