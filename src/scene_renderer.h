#pragma once

#include <GLExternalRAII/glfw_window_raii.h>
#include <GL3D/shader.h>

#include "mesh_builder.h"
#include "camera.h"

void draw_mesh(const Camera& cam, const MeshBuilder::Node& node, const MeshBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader) {
	const glm::mat4 global_transform = node.get_global_transform();;
	glm::mat4 view = cam.get_view_matrix();
	glm::mat4 projection = cam.get_projection_matrix();
	glm::mat4 transform_matrix = projection * view * global_transform;
	shader.set_uniform("uMat", transform_matrix);
	auto& material = mesh.material;
	if (material.diffuse_texture) { shader.set_texture("uDiffuse", *material.diffuse_texture, 0); }
	if (material.normal_texture) { shader.set_texture("uNormal", *material.normal_texture, 1); }
	if (material.roughness_texture) { shader.set_texture("uRoughness", *material.roughness_texture, 2); }
	if (material.metallic_texture) { shader.set_texture("uMetallic", *material.metallic_texture, 3); }
	mesh.mesh->draw(shader);
}
void draw_single_node(const Camera& cam, const MeshBuilder::Node& node, const GL3D::ShaderProgram& shader) {
	for (size_t i = 0; i < node.meshes.size(); i++) {
		draw_mesh(cam, node, node.meshes[i], shader);
	}
}
void draw_node(const Camera& cam, const MeshBuilder::Node& node, const GL3D::ShaderProgram& shader) {
	draw_single_node(cam, node, shader);
	for (size_t i = 0; i < node.child_nodes.size(); i++) {
		draw_node(cam, *node.child_nodes[i], shader);
	}
}
void draw_scene(const Camera& cam, const MeshBuilder::Scene& scene, const GL3D::ShaderProgram& shader) {
	draw_node(cam, *scene.root_node, shader);
}