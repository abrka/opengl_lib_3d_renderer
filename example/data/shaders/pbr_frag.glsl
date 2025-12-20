#version 330 core

in vec2 o_tex_coord;
out vec4 FragColor;

uniform sampler2D u_texture_diffuse;

struct DirectionalLight{
	vec3 color;
	float ambient_strength;
};
#define NUM_DIR_LIGHTS 1
uniform DirectionalLight u_dir_lights[NUM_DIR_LIGHTS];

void main()
{   
	vec3 diffuse_texture_color = texture2D(u_texture_diffuse, o_tex_coord).xyz;

	vec3 ambient_dir_light = vec3(0.0);
	for (int i = 0; i < NUM_DIR_LIGHTS; i++){
		vec3 ambient = diffuse_texture_color * u_dir_lights[i].ambient_strength * u_dir_lights[i].color;
		ambient_dir_light += ambient;
	}
	FragColor = vec4(ambient_dir_light, 1.0);
}