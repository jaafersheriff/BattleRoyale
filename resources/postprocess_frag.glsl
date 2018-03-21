#version 330 core

in vec2 f_texPos;
flat in int f_operation;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;

uniform float lifePercentage;

void onlyYellow() {
    color = vec4(1.f, 1.f, 0.f, 1.f);
}

void doBloom() {
    vec3 hdrColor = texture(f_texCol, f_texPos).rgb;      
    vec3 bloomColor = texture(f_bloomBlur, f_texPos).rgb;

    color = vec4(hdrColor + bloomColor, 1.0);

    // UI portion of bloom shader

    // Life UI code
    // Print the border
    bool inLifeBorder = false;
    vec2 lifeBorderDim = vec2(.8, .05);
    vec2 lifeBorderCenter = vec2(.5, .9);

    inLifeBorder = (
        f_texPos.x > (lifeBorderCenter.x - lifeBorderDim.x / 2) &&
        f_texPos.x < (lifeBorderCenter.x + lifeBorderDim.x / 2) &&
        f_texPos.y > (lifeBorderCenter.y - lifeBorderDim.y / 2) &&
        f_texPos.y < (lifeBorderCenter.y + lifeBorderDim.y / 2)
    );

    if(inLifeBorder) {
        color = vec4(0.f, 0.f, 0.f, 1.f);
    }

    // Print the life
    bool inLife = false;
    vec2 lifeDim = vec2(.75, 0.025);
    vec2 lifeCenter = vec2(.5, .9);

    inLife = (
        f_texPos.x > (lifeCenter.x - lifeDim.x / 2) &&
        f_texPos.x < (lifeCenter.x + lifeDim.x / 2) &&
        f_texPos.y > (lifeCenter.y - lifeDim.y / 2) &&
        f_texPos.y < (lifeCenter.y + lifeDim.y / 2)
    );

    // Use life percentage
    inLife = inLife && !(
       (f_texPos.x - (1 - lifeDim.x) / 2) / lifeDim.x > lifePercentage 
    );

    if(inLife) {
        color = vec4(0.f, 1.f, 0.f, 1.f) * lifePercentage +
            vec4(1.f, 0.f, 0.f, 1.f) * (1 - lifePercentage);
    }

    // Crosshair UI code
    bool inCrosshair = false;
    float crosshairLong = .02;
    float crosshairShort = .0025;

    // Determines if the texture position is in the crosshair
    inCrosshair = (
        f_texPos.x > (.50 - crosshairShort / 2) &&
        f_texPos.x < (.50 + crosshairShort / 2) &&
        f_texPos.y > (.50 - crosshairLong / 2) &&
        f_texPos.y < (.50 + crosshairLong / 2)
        ) || (
        f_texPos.x > (.50 - crosshairLong / 2) &&
        f_texPos.x < (.50 + crosshairLong / 2) &&
        f_texPos.y > (.50 - crosshairShort / 2) &&
        f_texPos.y < (.50 + crosshairShort / 2)
    );

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
    if (f_operation == 1) {
        doBloom();
    }
    else if(f_operation == 2) {
        copyTexture();
    }
    else {
        onlyYellow();
    }
}