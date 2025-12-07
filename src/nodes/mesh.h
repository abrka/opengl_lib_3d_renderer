#pragma once

#include <memory>

#include "GL3D/shader.h"
#include "GL3D/texture.h"

#include "node.h"
#include "IRenderable.h"
#include "mesh_builder.h"
#include "camera.h"

namespace Engine {
	class Mesh : public Node, public IRenderable {
	public:
		std::shared_ptr<MeshBuilder::Scene> scene{};
		std::shared_ptr<GL3D::ShaderProgram> shader{};
		glm::mat4 transform{1.0f};

		void render(const RenderContext& ctx) override {
			draw_node(ctx.cam, *scene->root_node);
		}
	private:
		void draw_mesh(const Camera& cam, const MeshBuilder::Node& node, const MeshBuilder::Mesh& mesh) {
			const glm::mat4 global_transform = node.get_global_transform() * transform;
			glm::mat4 view = cam.get_view_matrix();
			glm::mat4 projection = cam.get_projection_matrix();
			glm::mat4 transform_matrix = projection * view * global_transform;
			shader->set_uniform("uMat", transform_matrix);
			auto& material = mesh.material;
			GL3D::Texture* diffuse_texture = material.textures.at(aiTextureType_DIFFUSE).get();
			if (diffuse_texture) { shader->set_texture("uDiffuse", *diffuse_texture, 0); }
			//if (material.normal_texture) { shader->set_texture("uNormal", *material.normal_texture, 1); }
			//if (material.roughness_texture) { shader->set_texture("uRoughness", *material.roughness_texture, 2); }
			//if (material.metallic_texture) { shader->set_texture("uMetallic", *material.metallic_texture, 3); }
			mesh.mesh->draw(*shader);
		}
		void draw_single_node(const Camera& cam, const MeshBuilder::Node& node) {
			for (size_t i = 0; i < node.meshes.size(); i++) {
				draw_mesh(cam, node, node.meshes[i]);
			}
		}
		void draw_node(const Camera& cam, const MeshBuilder::Node& node) {
			draw_single_node(cam, node);
			for (size_t i = 0; i < node.child_nodes.size(); i++) {
				draw_node(cam, *node.child_nodes[i]);
			}
		}

	};
};