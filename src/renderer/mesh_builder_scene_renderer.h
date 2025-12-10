#pragma once

#include "render_context.h"
#include "mesh_builder.h"

namespace Renderer {
	namespace MeshBuilderSceneRenderer {
		void draw_mesh(const Camera& cam, const MeshBuilder::Node& node, const MeshBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader, glm::mat4 transform) {
			const glm::mat4 global_transform = node.get_global_transform() * transform;
			glm::mat4 view = cam.get_view_matrix();
			glm::mat4 projection = cam.get_projection_matrix();
			glm::mat4 transform_matrix = projection * view * global_transform;
			shader.set_uniform("uMat", transform_matrix);
			auto& material = mesh.material;
			GL3D::Texture* diffuse_texture = material.textures.at(aiTextureType_DIFFUSE).get();
			if (diffuse_texture) { shader.set_texture("uDiffuse", *diffuse_texture, 0); }
			mesh.mesh->draw(shader);
		}
		void draw_single_node(const Camera& cam, const MeshBuilder::Node& node, const GL3D::ShaderProgram& shader, glm::mat4 transform) {
			for (size_t i = 0; i < node.meshes.size(); i++) {
				draw_mesh(cam, node, node.meshes[i], shader, transform);
			}
		}
		void draw_node(const Camera& cam, const MeshBuilder::Node& node, const GL3D::ShaderProgram& shader, glm::mat4 transform) {
			draw_single_node(cam, node, shader, transform);
			for (size_t i = 0; i < node.child_nodes.size(); i++) {
				draw_node(cam, *node.child_nodes[i], shader, transform);
			}
		}
		void render(const RenderContext& ctx, const MeshBuilder::Scene& scene, const GL3D::ShaderProgram& shader, glm::mat4 transform) {
			draw_node(ctx.cam, *scene.root_node, shader, transform);
		}
	}
}
