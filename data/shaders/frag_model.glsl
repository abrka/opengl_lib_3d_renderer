#version 330 core

in vec2 oTexCoord;
out vec4 FragColor;

uniform sampler2D uDiffuse;

void main()
{   
	vec4 diff = texture2D(uDiffuse, oTexCoord);
	FragColor = diff;
}