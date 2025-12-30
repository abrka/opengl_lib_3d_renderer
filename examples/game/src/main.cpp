#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/renderer.h"
#include "engine/components/components.h"
#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"
#include "input/input.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


int main() {
	const std::string asset_dir = std::string(TOSTRING(ASSET_DIR)) + "/";
	const std::string engine_asset_dir = std::string(TOSTRING(ENGINE_ASSET_DIR)) + "/";

	entt::registry entt_registry{};
	
	GLExternalRAII::Window window{ 800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR };
	window.key_callback = [&window](int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window.glfw_window, true);
		}
	};
	Input::Input input_system{ *window.glfw_window };
	input_system.disable_cursor();

	Renderer::Renderer3D renderer{ window, entt_registry };
	window.framebuffer_size_callback = [&renderer](int width, int height) {
		renderer.on_window_resize(width, height);
	};

	using namespace entt::literals;
	entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> entt_shader_cache{};
	auto pbr_shader = entt_shader_cache.load("pbr"_hs, engine_asset_dir + "shaders/pbr_frag.glsl", engine_asset_dir + "shaders/pbr_vertex.glsl").first->second;

	entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader> entt_mesh_cache{};
	auto candle_scene = entt_mesh_cache.load("candle"_hs, asset_dir + "meshes/candle/brass_candleholders_1k.gltf").first->second;
	
	entt::entity root_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(root_entity, "root");
	entt_registry.emplace<Engine::RootComponent>(root_entity);

	entt::entity candle_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(candle_entity, "candle");
	entt_registry.emplace<Engine::MeshComponent>(candle_entity, candle_scene);
	entt_registry.emplace<Engine::ShaderComponent>(candle_entity, pbr_shader);
	entt_registry.emplace<Engine::TransformComponent>(candle_entity);
	Engine::add_child(entt_registry, root_entity, candle_entity);

	entt::entity camera_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(camera_entity, "camera");
	Engine::CameraComponent camera_component{};
	camera_component.camera.position = { 0, 0, -1 };
	entt_registry.emplace<Engine::CameraComponent>(camera_entity, camera_component);
	Engine::add_child(entt_registry, root_entity, camera_entity);

	entt::entity point_light_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(point_light_entity, "point light");
	Engine::PointLightComponent point_light_comp{};
	point_light_comp.light.color = glm::vec3(1.0f);
	point_light_comp.light.ambient_strength = 0.1f;
	point_light_comp.light.diffuse_strength = 1.0f;
	point_light_comp.light.specular_strength = 0.5f;
	entt_registry.emplace<Engine::PointLightComponent>(point_light_entity, point_light_comp);
	entt_registry.emplace<Engine::TransformComponent>(point_light_entity);
	Engine::add_child(entt_registry, root_entity, point_light_entity);

	auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
	for (auto [entity, mesh_component] : entt_view_meshes.each()) {
		auto materials = mesh_component.scene->get_all_materials();
		for (AssetBuilder::Material* material : materials) {
			material->set_uniform("u_material.specular_alpha", 32.0f);
			material->set_uniform("u_material.ambient_strength", 1.0f);
			material->set_uniform("u_material.diffuse_strength", 1.0f);
			material->set_uniform("u_material.specular_strength", 1.0f);
		}
	}

	while (window.is_running()) {
		double prev_time = glfwGetTime();

		renderer.render();

		double current_time = glfwGetTime();
		double delta = current_time - prev_time;


		double fps = 1 / delta;
		const double fps_set_title_delay = 0.5;
		static double last_time_fps_was_set{};
		if (current_time - last_time_fps_was_set > fps_set_title_delay) {
			std::string title = "fps: " + std::to_string(fps);
			glfwSetWindowTitle(window.glfw_window, title.c_str());
			last_time_fps_was_set = current_time;
		}

	}
}