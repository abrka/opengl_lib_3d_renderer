#pragma once

#include "engine/components/components.h"
#include "asset_builder.h"

namespace Renderer {
	namespace detail {
		void draw_mesh(const AssetBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader) {
			mesh.material.update_all_uniforms(shader);
			mesh.mesh->draw(shader);
		}
		void render_scene(const AssetBuilder::Scene& scene, const GL3D::ShaderProgram& shader) {
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

		void set_mvp_uniforms_single(AssetBuilder::Mesh& mesh, glm::mat4 transform, const Renderer::Camera& camera) {
			glm::mat4 model = mesh.node->get_global_transform() * transform;
			glm::mat4 view = camera.get_view_matrix();
			glm::mat4 projection = camera.get_projection_matrix();
			mesh.material.set_uniform("u_mat_model", model);
			mesh.material.set_uniform("u_mat_view", view);
			mesh.material.set_uniform("u_mat_projection", projection);
		}
		void set_mvp_uniforms(AssetBuilder::Scene& scene, glm::mat4 transform, const Camera& camera) {
			for (auto* mesh : scene.get_all_meshes()) {
				set_mvp_uniforms_single(*mesh, transform, camera);
			}
		}
		void set_mvp_uniforms_system(entt::registry& entt_registry) {
			auto entt_view_mesh = entt_registry.view<Engine::MeshComponent, Engine::TransformComponent>();
			auto entt_view_camera = entt_registry.view<Engine::CameraComponent>();
			for (auto [entity, mesh_component, transform_component] : entt_view_mesh.each()) {
				for (auto [entity, camera_component] : entt_view_camera.each()) {
					set_mvp_uniforms(*mesh_component, transform_component.transform, camera_component.camera);
				}
			}
		}

		void set_point_light_uniform(AssetBuilder::Material& material, Renderer::PointLight& point_light, glm::mat4 transform, size_t i)
		{
			std::string point_light_uniform_name = "u_point_lights[" + std::to_string(i) + "]";
			material.set_uniform(point_light_uniform_name + ".color", point_light.color);
			glm::vec3 position = transform[3];
			material.set_uniform(point_light_uniform_name + ".position", position);
			material.set_uniform(point_light_uniform_name + ".ambient_strength", point_light.ambient_strength);
			material.set_uniform(point_light_uniform_name + ".diffuse_strength", point_light.diffuse_strength);
			material.set_uniform(point_light_uniform_name + ".specular_strength", point_light.specular_strength);
			material.set_uniform(point_light_uniform_name + ".constant", point_light.constant);
			material.set_uniform(point_light_uniform_name + ".linear", point_light.linear);
			material.set_uniform(point_light_uniform_name + ".quadratic", point_light.quadratic);
		}

		void set_light_uniforms_system(entt::registry& entt_registry, size_t num_point_lights) {
			auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
			for (auto [entity, mesh_component] : entt_view_meshes.each()) {
				auto materials = mesh_component->get_all_materials();
				for (AssetBuilder::Material* material : materials) {
					auto entt_view_point_lights = entt_registry.view<Engine::PointLightComponent, Engine::TransformComponent>();
					size_t i = 0;
					for (auto [entity, point_light_component, transform_component] : entt_view_point_lights.each()) {
						if (i >= num_point_lights) {
							return;
						}
						Renderer::PointLight point_light = point_light_component.light;
						set_point_light_uniform(*material, point_light, transform_component.transform, i);
						i++;
					}
					while (i < num_point_lights) {
						PointLight empty_light{};
						set_point_light_uniform(*material, empty_light, glm::mat4(1.0f), i);
						i++;
					}

				}
			}
		}

		void set_camera_uniform_system(entt::registry& entt_registry) {
			auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
			for (auto [entity, mesh_component] : entt_view_meshes.each()) {
				auto materials = mesh_component->get_all_materials();
				for (AssetBuilder::Material* material : materials) {
					auto entt_view_camera = entt_registry.view<Engine::CameraComponent>();
					for (auto [entity, camera_component] : entt_view_camera.each()) {
						material->set_uniform("u_camera.position", camera_component.camera.position);
					}
				}
			}
		}
	}
}
