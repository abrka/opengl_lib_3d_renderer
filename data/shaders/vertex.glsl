#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 outTexCoord;

uniform mat4 uMat;

void main()
{						
	outTexCoord = aTexCoord;
	gl_Position = uMat * vec4(aPos, 1.0);
}