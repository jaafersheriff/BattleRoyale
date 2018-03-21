#version 330 core

in vec2 f_texPos;
flat in int f_operation;

out vec4 color;

uniform sampler2D f_texCol;
uniform sampler2D f_bloomBlur;

uniform float lifePercentage;
uniform float ammoPercentage;

const vec3 k_minLifeColor = vec3(1.0, 0.0, 0.0);
const vec3 k_maxLifeColor = vec3(0.0, 1.0, 0.0);
const vec3 k_ammoColor = vec3(0.0, 0.5, 1.0);
const vec3 k_backdropColor = vec3(0.0, 0.0, 0.0);

const vec2 k_lifeCenter = vec2(0.2, 0.1);
const vec2 k_lifeDim = vec2(0.3, 0.025);
const vec2 k_ammoCenter = vec2(0.8, 0.1);
const vec2 k_ammoDim = vec2(0.3, 0.025);
const float k_border = 0.003;

void onlyYellow() {
    color = vec4(1.0, 1.0, 0.0, 1.0);
}

bool inLifeBackdrop() {
    return
        f_texPos.x > (k_lifeCenter.x - k_lifeDim.x * 0.5 - k_border) &&
        f_texPos.x < (k_lifeCenter.x + k_lifeDim.x * 0.5 + k_border) &&
        f_texPos.y > (k_lifeCenter.y - k_lifeDim.y * 0.5 - k_border) &&
        f_texPos.y < (k_lifeCenter.y + k_lifeDim.y * 0.5 + k_border);
}

bool inLife() {
    return
        f_texPos.x > (k_lifeCenter.x - k_lifeDim.x * 0.5) &&
        f_texPos.x < (k_lifeCenter.x + k_lifeDim.x * 0.5) &&
        f_texPos.y > (k_lifeCenter.y - k_lifeDim.y * 0.5) &&
        f_texPos.y < (k_lifeCenter.y + k_lifeDim.y * 0.5) &&
        // Use life percentage
        !((f_texPos.x - (k_lifeCenter.x - k_lifeDim.x * 0.5)) / k_lifeDim.x > lifePercentage);
}

bool inAmmoBackdrop() {
    return
        f_texPos.x > (k_ammoCenter.x - k_ammoDim.x * 0.5 - k_border) &&
        f_texPos.x < (k_ammoCenter.x + k_ammoDim.x * 0.5 + k_border) &&
        f_texPos.y > (k_ammoCenter.y - k_ammoDim.y * 0.5 - k_border) &&
        f_texPos.y < (k_ammoCenter.y + k_ammoDim.y * 0.5 + k_border);
}

bool inAmmo() {
    return
        f_texPos.x > (k_ammoCenter.x - k_ammoDim.x * 0.5) &&
        f_texPos.x < (k_ammoCenter.x + k_ammoDim.x * 0.5) &&
        f_texPos.y > (k_ammoCenter.y - k_ammoDim.y * 0.5) &&
        f_texPos.y < (k_ammoCenter.y + k_ammoDim.y * 0.5) &&
        // Use ammo percentage
        !((f_texPos.x - (k_ammoCenter.x - k_ammoDim.x * 0.5)) / k_ammoDim.x > lifePercentage);
}

void doBloom() {
    vec3 hdrColor = texture(f_texCol, f_texPos).rgb;      
    vec3 bloomColor = texture(f_bloomBlur, f_texPos).rgb;

    color = vec4(hdrColor + bloomColor, 1.0);

    // UI portion of bloom shader

    // Life
    if (inLifeBackdrop()) {
        if (inLife()) {
            color.rgb = mix(k_minLifeColor, k_maxLifeColor, lifePercentage);
        }
        else {
            color.rgb = k_backdropColor;			
        }
    }
    // Ammo
    else if (inAmmoBackdrop()) {
        if (inAmmo()) {
            color.rgb = k_ammoColor;
        }
        else {
            color.rgb = k_backdropColor;			
        }
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
        color = vec4(1.0 - color.x, 1.0 - color.y, 1.0 - color.z, 1.0);
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