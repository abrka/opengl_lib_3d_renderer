#pragma once

#include "engine/components/components.h"
#include "mesh_builder.h"

namespace Renderer {
	namespace detail {
		template<typename T>
		T* get_first_component_of_type(entt::registry& entt_registry) {
			auto view = entt_registry.view<T>();
			for (auto [entity, component] : view.each()) {
				return &component;
			}
			return nullptr;
		}
		void draw_mesh(const MeshBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader) {
			mesh.material.update_all_uniforms(shader);
			mesh.mesh->draw(shader);
		}

		void render_scene(const MeshBuilder::Scene& scene, const GL3D::ShaderProgram& shader) {
			for (auto* mesh : scene.get_all_meshes()) {
				draw_mesh(*mesh, shader);
			}
		}
		void render_mesh_system(entt::registry& entt_registry) {
			auto view = entt_registry.view<const Engine::MeshComponent, const Engine::ShaderComponent>();
			for (auto [entity, mesh_component, shader_component] : view.each()) {
				render_scene( *mesh_component, *shader_component);
			}
		}

		void set_mvp_uniforms_single(MeshBuilder::Mesh& mesh, glm::mat4 transform, const Renderer::Camera& camera) {
			glm::mat4 model = mesh.node->get_global_transform() * transform;
			glm::mat4 view = camera.get_view_matrix();
			glm::mat4 projection = camera.get_projection_matrix();
			mesh.material.set_uniform("u_mat_model", model);
			mesh.material.set_uniform("u_mat_view", view);
			mesh.material.set_uniform("u_mat_projection", projection);
		}

		void set_mvp_uniforms(MeshBuilder::Scene& scene, glm::mat4 transform, const Camera& camera) {
			for (auto* mesh : scene.get_all_meshes()) {
				set_mvp_uniforms_single(*mesh, transform, camera);
			}
		}
		void set_mvp_uniforms_system(entt::registry& entt_registry) {
			auto entt_view = entt_registry.view<Engine::MeshComponent, Engine::TransformComponent>();
			for (auto [entity, mesh_component, transform_component] : entt_view.each()) {
				const Engine::CameraComponent* camera_component = get_first_component_of_type<const Engine::CameraComponent>(entt_registry);
				if (!camera_component) {
					return;
				}
				set_mvp_uniforms(*mesh_component, transform_component.transform, camera_component->camera);
			}
		}

		// void set_light_uniforms_system(entt::registry& entt_registry) {
		// 	auto entt_view = entt_registry.view<Engine::MeshComponent>();
		// 	for (auto [entity, mesh_component] : entt_view.each()) {
		// 		auto materials = mesh_component->get_all_materials();
		// 		for (MeshBuilder::Material* material : materials) {
		// 			for (size_t i = 0; i < render_ctx.point_lights.size(); i++) {
		// 				Renderer::PointLight point_light = render_ctx.point_lights[i];
		// 				std::string dir_light_uniform = "u_point_lights[" + std::to_string(i) + "]";
		// 				material->set_uniform(dir_light_uniform + ".color", point_light.color);
		// 				material->set_uniform(dir_light_uniform + ".position", point_light.position);
		// 				material->set_uniform(dir_light_uniform + ".ambient_strength", point_light.ambient_strength);
		// 			}
		// 		}
		// 	}
		// }


	}
}
