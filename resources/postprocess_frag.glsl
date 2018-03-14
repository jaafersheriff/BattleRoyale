#version 330 core

in vec2 f_screenPos;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;
uniform float exposure;

void main()
{             
    const float gamma = 2.2;
    vec3 hdrColor = texture(f_texCol, f_screenPos).rgb;      
    vec3 bloomColor = texture(f_bloomBlur, f_screenPos).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));

	vec3 additive = hdrColor + bloomColor*0.5;
    color = vec4(additive, 1.0);
}  

/*
void main() {
	color = texture(f_texCol, f_screenPos);
}*/