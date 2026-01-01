#version 330 core

struct Camera {
	vec3 position;
};
uniform Camera u_camera;

struct Material{
	vec3 base_color;
	
	vec3 ambient_color;
	sampler2D texture_ambient;

	vec3 diffuse_color;
	sampler2D texture_diffuse;
	int has_texture_diffuse;

	vec3 specular_color;
	sampler2D texture_specular;

	vec3 emissive_color;
	sampler2D texture_emissive;

	float ambient_strength;
	float diffuse_strength;
	float specular_strength;

	float specular_alpha;
};
uniform Material u_material;

struct PointLight{
	vec3 position;
	vec3 color;
	
	float ambient_strength;
	float diffuse_strength;
	float specular_strength;

	float constant;
	float linear;
	float quadratic;
};
#define MAX_NUM_POINT_LIGHTS 10
uniform PointLight u_point_lights[MAX_NUM_POINT_LIGHTS];
uniform int u_num_point_lights;

in VS_OUT{
	vec2 tex_coord;
	vec3 position_world;
	vec3 normal_world;
} fs_in;

out vec4 FragColor;

void main()
{   
	vec3 diffuse_texture_color = u_material.base_color;
	if (bool(u_material.has_texture_diffuse)){
		 diffuse_texture_color *= texture2D(u_material.texture_diffuse, fs_in.tex_coord).xyz;
	}

	vec3 ambient_point_light = vec3(0.0);
	vec3 diffuse_point_light = vec3(0.0);
	vec3 specular_point_light = vec3(0.0);

	
	for (int i = 0; i < u_num_point_lights; i++){
		float dist = length(u_point_lights[i].position - fs_in.position_world);
		float attenuation = 1.0 / (u_point_lights[i].constant + u_point_lights[i].linear * dist + 
    		    u_point_lights[i].quadratic * (dist * dist)); 

		vec3 ambient = u_material.ambient_strength * attenuation * u_point_lights[i].ambient_strength * u_point_lights[i].color;
		ambient_point_light += ambient;

		vec3 L = normalize(u_point_lights[i].position - fs_in.position_world);
		vec3 N = normalize(fs_in.normal_world);

		float diffuse_factor = max(dot(N, L), 0.0);
		vec3 diffuse = diffuse_factor * attenuation * u_material.diffuse_strength * u_point_lights[i].diffuse_strength * u_point_lights[i].color;
		diffuse_point_light += diffuse;

		vec3 V = normalize(u_camera.position - fs_in.position_world);
		vec3 R = reflect(-L , N);
		float specular_factor = pow(max(dot(V, R), 0.0), u_material.specular_alpha);
		vec3 specular =  specular_factor * attenuation * u_material.specular_strength * u_point_lights[i].specular_strength * u_point_lights[i].color;
		
		specular_point_light += specular;
	}

	vec3 ambient_tot = ambient_point_light;
	vec3 diffuse_tot = diffuse_point_light;
	vec3 specular_tot = specular_point_light;

	vec3 color = diffuse_texture_color * (ambient_tot + diffuse_tot) + specular_tot; 
	FragColor = vec4(color, 1.0);
}