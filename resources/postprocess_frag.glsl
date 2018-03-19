#version 330 core

in vec2 f_texPos;
flat in int f_operation;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;
uniform float exposure;

void onlyYellow() {
    color = vec4(1.f, 1.f, 0.f, 1.f);
}

void main()
{             
    if(f_operation == 1) {
        vec3 hdrColor = texture(f_texCol, f_texPos).rgb;      
        vec3 bloomColor = texture(f_bloomBlur, f_texPos).rgb;

        vec3 additive = hdrColor + bloomColor*exposure;
        color = vec4(additive, 1.0);
    }
    if(f_operation == 0) {
        onlyYellow();
    }
}