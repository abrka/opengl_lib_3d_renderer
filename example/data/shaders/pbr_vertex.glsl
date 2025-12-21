#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

uniform mat4 u_mat_model;
uniform mat4 u_mat_view;
uniform mat4 u_mat_projection;

out VS_OUT
{
	vec2 tex_coord;
	vec3 position_world;
	vec3 normal_world;
} vs_out;

void main()
{						
	vs_out.tex_coord = a_tex_coord;
	vs_out.position_world = vec3(u_mat_model * vec4(a_pos, 1.0));
	vs_out.normal_world = mat3(transpose(inverse(u_mat_model))) * a_normal;
	gl_Position = u_mat_projection * u_mat_view * u_mat_model * vec4(a_pos, 1.0);
}