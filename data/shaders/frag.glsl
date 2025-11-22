#version 330 core

out vec4 FragColor;
in vec2 outTexCoord;
uniform sampler2D tex1;

void main()
{   
	vec4 c = texture2D(tex1, outTexCoord);
	FragColor = c;
}