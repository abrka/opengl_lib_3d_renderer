#version 330 core

in vec2 uv;
out vec4 FragColor;
uniform sampler2D screen_texture;

void main()
{   
	vec3 screen_color = texture2D(screen_texture, uv).xyz;
	FragColor = vec4(screen_color,1.0);
}