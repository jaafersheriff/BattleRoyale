#version 330 core

in vec2 f_screenPos;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;
uniform float exposure;

void main()
{             
    vec3 hdrColor = texture(f_texCol, f_screenPos).rgb;      
    vec3 bloomColor = texture(f_bloomBlur, f_screenPos).rgb;

	vec3 additive = hdrColor + bloomColor*exposure;
    color = vec4(additive, 1.0);
}  

/*
void main() {
	color = texture(f_texCol, f_screenPos);
}*/