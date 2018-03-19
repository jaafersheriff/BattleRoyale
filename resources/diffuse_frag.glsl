#version 330 core

in vec3 fragPos;
in vec4 fragLPos;
in vec3 fragNor;
in vec2 texCoords;

uniform sampler2D shadowMap;
uniform float shadowAmbience;

uniform float matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float shine;

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
                specularContrib = pow(texelFetch(cellSpecularScales, i, 0).r, shine);
                break;
            }
        }
    }
    /* Blinn-Phong shading */
    else {
        vec3 H = (L + V) / 2.0;
        diffuseContrib = clamp(lambert, matAmbient, 1.0);
        specularContrib = pow(max(dot(H, N), 0.0), shine);
    }

    /* Base color + shadow */
    vec4 shadowCoords = fragLPos * 0.5 + 0.5;
    float visibility = 1.0;
    if (shadowCoords.x >= 0.0 && shadowCoords.x <= 1.0 && shadowCoords.y >= 0.0 && shadowCoords.y <= 1.0 && 
            shadowCoords.z > texture(shadowMap, shadowCoords.xy).r + 0.005) {
        visibility -= (shadowCoords.w * (1.0 - shadowAmbience));
    }
    vec3 bColor = vec3(diffuseColor*diffuseContrib + matSpecular*specularContrib) * visibility;

    /* Silhouettes */
    float edge = (isToon && (clamp(dot(N, V), 0.0, 1.0) < silAngle)) ? 0.0 : 1.0;

    color = vec4(edge * bColor, alpha);
}
