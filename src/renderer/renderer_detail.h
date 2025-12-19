#pragma once

#include "render_context.h"
#include "mesh_builder.h"

namespace Renderer {
	namespace detail {
		void draw_mesh(const Camera& cam, const MeshBuilder::Node& node, const MeshBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader) {
			mesh.material.set_all_uniforms(shader);
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
		void render_scene(const RenderContext& ctx, const MeshBuilder::Scene& scene, const GL3D::ShaderProgram& shader) {
			draw_node(ctx.cam, *scene.root_node, shader);
		}

		void set_mvp_uniforms(const RenderContext& ctx, MeshBuilder::Mesh& mesh, MeshBuilder::Node& node, glm::mat4 transform) {
			glm::mat4 model = node.get_global_transform() * transform;
			glm::mat4 view = ctx.cam.get_view_matrix();
			glm::mat4 projection = ctx.cam.get_projection_matrix();
			mesh.material.uniforms_mat4["u_mat_model"] = model;
			mesh.material.uniforms_mat4["u_mat_view"] = view;
			mesh.material.uniforms_mat4["u_mat_projection"] = projection;
		}
		void set_mvp_uniforms(const RenderContext& ctx, MeshBuilder::Node& node, glm::mat4 transform) {
			for (size_t i = 0; i < node.meshes.size(); i++)
			{
				set_mvp_uniforms(ctx, node.meshes[i], node, transform);
			}
			for (size_t i = 0; i < node.child_nodes.size(); i++)
			{
				set_mvp_uniforms(ctx, *node.child_nodes[i], transform);
			}
		}
		void set_mvp_uniforms(const RenderContext& ctx, MeshBuilder::Scene& scene, glm::mat4 transform) {
			set_mvp_uniforms(ctx, *scene.root_node, transform);
		}
	}
}
