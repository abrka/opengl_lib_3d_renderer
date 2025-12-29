#pragma once

#include "engine/components/components.h"
#include "asset_builder.h"

namespace Renderer {
	namespace detail {
		template<typename MapType>
		void update_uniforms(const GL3D::ShaderProgram& shader, MapType& uniform_map) {
			for (const auto& [uniform_name, value] : uniform_map) {
				shader.set_uniform(uniform_name, value);
			}
		}
		void update_uniforms(const GL3D::ShaderProgram& shader, std::map<std::string, AssetBuilder::Texture> textures_map) {
			for (const auto& [uniform_name, texture] : textures_map) {
				shader.set_texture(uniform_name, *texture.texture, texture.texture_unit);
			}
		}
		void update_all_uniforms(const AssetBuilder::Material& material, const GL3D::ShaderProgram& shader) {
			update_uniforms(shader, material.uniforms_texture);
			update_uniforms(shader, material.uniforms_int);
			update_uniforms(shader, material.uniforms_float);
			update_uniforms(shader, material.uniforms_vec3);
			update_uniforms(shader, material.uniforms_mat4);
		}
		void draw_mesh(const AssetBuilder::Mesh& mesh, const GL3D::ShaderProgram& shader) {
			update_all_uniforms(mesh.material,shader);
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
				if (!mesh_component.scene) {
					continue;
				}
				render_scene( *mesh_component.scene, *shader_component.shader);
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
					if (!mesh_component.scene) {
						continue;
					}
					set_mvp_uniforms(*mesh_component.scene, transform_component.transform, camera_component.camera);
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
				if (!mesh_component.scene) {
					continue;
				}
				auto materials = mesh_component.scene->get_all_materials();
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
					size_t num_point_lights = i;
					material->set_uniform("u_num_point_lights", (int)num_point_lights);
				}
			}
		}

		void set_camera_uniform_system(entt::registry& entt_registry) {
			auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
			for (auto [entity, mesh_component] : entt_view_meshes.each()) {
				if (!mesh_component.scene) {
					continue;
				}
				auto materials = mesh_component.scene->get_all_materials();
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
