#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;

uniform mat4 u_mat_model;
uniform mat4 u_mat_view;
uniform mat4 u_mat_projection;

out vec2 o_tex_coord;

void main()
{						
	o_tex_coord = a_tex_coord;
	gl_Position = u_mat_projection * u_mat_view * u_mat_model * vec4(a_pos, 1.0);
}