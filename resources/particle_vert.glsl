#version 330 core

const int MAX_ORIENTATIONS = 21;

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
layout(location = 3) in vec3 particleOffset;
//layout(location = 4) in int orientationID;

uniform mat4 P;
uniform mat4 M;
uniform mat4 randomMs[MAX_ORIENTATIONS];
uniform mat4 V;
uniform mat3 N;
uniform mat3 randomNs[MAX_ORIENTATIONS];
uniform vec3 cameraPos;
uniform bool randomOrientation;

out vec3 worldPos;
out vec3 fragNor;
out vec2 texCoords;

void main() {
	if(randomOrientation){
		//worldPos = vec3(randomMs[orientationID + 1] * vec4(vertPos, 1.0f)) + particleOffset;
		//fragNor = randomNs[orientationID + 1] * vertNor;
		worldPos = vec3(randomMs[gl_InstanceID % 10 + 1] * vec4(vertPos, 1.0f)) + particleOffset;
		fragNor = randomNs[gl_InstanceID % 10 + 1] * vertNor;
	}
	else{
		worldPos = vec3(M * vec4(vertPos, 1.0f)) + particleOffset;
		fragNor = N * vertNor;
	}
	
	texCoords = vertTex;
    gl_Position = P * V * vec4(worldPos, 1.0f);    
}
