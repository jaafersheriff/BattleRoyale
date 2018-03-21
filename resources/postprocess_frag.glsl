#version 330 core

in vec2 f_texPos;
flat in int f_operation;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;
uniform float exposure;

uniform float lifePercentage;

void onlyYellow() {
    color = vec4(1.f, 1.f, 0.f, 1.f);
}

void doBloom() {
    vec3 hdrColor = texture(f_texCol, f_texPos).rgb;      
    vec3 bloomColor = texture(f_bloomBlur, f_texPos).rgb;

    vec3 additive = hdrColor + bloomColor*exposure;
    color = vec4(additive, 1.0);

    // UI portion of bloom shader

    // Life UI code
    if(f_texPos.x < .03) {
        color = vec4(0.f, 0.f, 0.f, 1.f);
    }

    if(f_texPos.x < .02 && f_texPos.y < lifePercentage) {
        color = vec4(0.f, 1.f, 0.f, 1.f) * lifePercentage +
            vec4(1.f, 0.f, 0.f, 1.f) * (1 - lifePercentage);
    }

    // Crosshair UI code
    bool inCrosshair = false;
    float crosshairLong = .02;
    float crosshairShort = .0025;

    // Vertical portion of crosshair
    if(f_texPos.x > (.50 - crosshairShort / 2) &&
        f_texPos.x < (.50 + crosshairShort / 2) &&
        f_texPos.y > (.50 - crosshairLong / 2) &&
        f_texPos.y < (.50 + crosshairLong / 2)) {
        inCrosshair = true;
    }

    // Horizontal portion of crosshair
    if(f_texPos.x > (.50 - crosshairLong / 2) &&
        f_texPos.x < (.50 + crosshairLong / 2) &&
        f_texPos.y > (.50 - crosshairShort / 2) &&
        f_texPos.y < (.50 + crosshairShort / 2)) {
        inCrosshair = true;
    }

    // Finally...
    if(inCrosshair) {
        color = vec4(1.f - color.x, 1.f - color.y, 1.f - color.z, 1.f);
    }
}

void copyTexture() {
    color = texture(f_texCol, f_texPos);
}

void main()
{             
    if(f_operation == 1) {
        doBloom();
    }
    else if(f_operation == 2) {
        copyTexture();
    }
    else {
        onlyYellow();
    }
}