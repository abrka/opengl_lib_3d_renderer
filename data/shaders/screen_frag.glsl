#version 330 core

in vec2 uv;
out vec4 FragColor;
uniform sampler2D screen_texture;

void main()
{   
	vec3 screen_color = texture2D(screen_texture, uv).xyz;
	vec3 c = vec3(1.0) - screen_color;
	FragColor = vec4(c,1.0);
}