#version 330 core

in vec3 worldPos;
in vec3 fragNor;
in vec2 texCoords;

uniform float matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float shine;

uniform vec3 camPos;
uniform vec3 lightDir;

uniform sampler2D textureImage;
uniform bool usesTexture;

//uniform bool isToon;
//uniform float silAngle;
// float numCells;
// sampler1D cellIntensities;
//uniform sampler1D cellDiffuseScales;
//uniform sampler1D cellSpecularScales;

out vec4 color;

void main() {
    vec3 viewDir = camPos - worldPos;
    vec3 V = normalize(viewDir);
    vec3 L = normalize(lightDir);
    vec3 N = normalize(fragNor);

    /* Base color */
    vec3 diffuseColor = matDiffuse;
    if (usesTexture) {
        diffuseColor = vec3(texture(textureImage, texCoords));
    }

    float lambert = dot(L, N);
    float diffuseContrib, specularContrib;

    /* Cell shading */
    /*if (isToon) {
        for(int i = 0; i < numCells; i++) {
            if(lambert > texelFetch(cellIntensities, i, 0).r) {
                diffuseContrib = texelFetch(cellDiffuseScales, i, 0).r;
                specularContrib = pow(texelFetch(cellSpecularScales, i, 0).r, shine);
                break;
            }
        }
    }*/
    /* Blinn-Phong shading */
    //else {
        vec3 H = (L + V) / 2.0;
        diffuseContrib = clamp(lambert, matAmbient, 1.0);
        specularContrib = pow(max(dot(H, N), 0.0), shine);
    //}

    /* Base color */
    vec3 bColor = vec3(diffuseColor*diffuseContrib + matSpecular*specularContrib);

    /* Silhouettes */
    //float edge = (isToon && (clamp(dot(N, V), 0.0, 1.0) < silAngle)) ? 0.0 : 1.0;
	float edge = 1.0f;
    color = vec4(edge * bColor, 1.0);
}