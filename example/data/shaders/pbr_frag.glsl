#version 330 core

in vec2 o_tex_coord;
out vec4 FragColor;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_base_color;

void main()
{   
	vec4 diff = texture2D(u_texture_diffuse, o_tex_coord);
	FragColor = diff;
}