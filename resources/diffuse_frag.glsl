#version 330 core

in vec3 fragPos;
in vec4 fragLPos;
in vec3 fragNor;
in vec2 texCoords;

uniform sampler2D shadowMap;

uniform float ambience;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShine;

uniform vec3 camPos;
uniform vec3 lightDir;

uniform sampler2D textureImage;
uniform bool usesTexture;

uniform bool isToon;
uniform float silAngle;
uniform float numCells;
uniform sampler1D cellIntensities;
uniform sampler1D cellDiffuseScales;
uniform sampler1D cellSpecularScales;

layout(location = 0) out vec4 color;
layout (location = 1) out vec4 BrightColor;

float inShade(vec3 lPos, float bias) {
    float worldDepth = texture(shadowMap, lPos.xy).r;
    float lightDepth = lPos.z;
    if (lightDepth > worldDepth + bias) {
        return 1.0;
    }
    return 0.0;
}

void main() {
    vec3 viewDir = camPos - fragPos;
    vec3 V = normalize(viewDir);
    vec3 L = normalize(-lightDir);
    vec3 N = normalize(fragNor);

    /* Base color */
    vec3 diffuseColor = matDiffuse;
    float alpha = 1.0;
    if (usesTexture) {
        vec4 texColor = vec4(texture(textureImage, texCoords));
        if (texColor.a < 0.1) {
            discard; 
        }
        diffuseColor = texColor.xyz;
        alpha = texColor.a;
    }

    float lambert = dot(L, N);
    float diffuseContrib, specularContrib;

    /* Cell shading */
    if (isToon) {
        for(int i = 0; i < numCells; i++) {
            if(lambert > texelFetch(cellIntensities, i, 0).r) {
                diffuseContrib = texelFetch(cellDiffuseScales, i, 0).r;
                specularContrib = pow(texelFetch(cellSpecularScales, i, 0).r, matShine);
                break;
            }
        }
    }
    /* Blinn-Phong shading */
    else {
        vec3 H = (L + V) / 2.0;
        diffuseContrib = clamp(lambert, ambience, 1.0);
        specularContrib = pow(max(dot(H, N), 0.0), matShine);
    }

    /* Base color + shadow */
    vec3 shift = fragLPos.xyz * 0.5 + 0.5;
    float shade = 0.0;
    if (shift.x >= 0.0 && shift.x <= 1.0 && shift.y >= 0.0 && shift.y <= 1.0) {
        shade = inShade(shift, 0.01);
    }
    vec3 bColor = vec3(diffuseColor*diffuseContrib + matSpecular*specularContrib) * max((1 - shade), ambience);

    /* Silhouettes */
    float edge = (isToon && (clamp(dot(N, V), 0.0, 1.0) < silAngle)) ? 0.0 : 1.0;

    color = vec4(edge * bColor, alpha);

    float brightness = max(((color.r+color.g+color.b)/3.0 - 0.6)/0.4, 0.0f);

    BrightColor.rgb = vec3(brightness);;

    BrightColor.a = 1;

    /*
    if(brightness > 0.6)
        BrightColor = vec4(color.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    */
}
