#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sol/sol.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "renderer/renderer.h"
#include "engine/components/components.h"
#include "engine/loaders/loaders.h"
#include "engine/systems/systems.h"
#include "engine/serialize/entt_registry_serializer.h"
#include "input/input.h"
#include "physics_wrapper/world.h"
#include "reflect/reflect.h"
#include "reflect/reflect_sol.h"


int main() {
	sol::state sol_state{};
	sol_state.open_libraries(sol::lib::base, sol::lib::package);
	Reflect::register_sol_usertypes(sol_state);

	Physics::World physics_world{};

	entt::registry entt_registry{};
	Reflect::register_all_components();

	GLExternalRAII::Window window{ 800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR };
	window.key_callback = [&window](int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window.glfw_window, true);
		}
		};
	Input::Input input_system{ *window.glfw_window };
	sol_state["Input"] = &input_system;

	Renderer::Renderer3D renderer{ window, entt_registry };
	window.framebuffer_size_callback = [&renderer](int width, int height) {
		renderer.on_window_resize(width, height);
		};

	Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer{};
	const char* filepath_c_str = tinyfd_openFileDialog("Scene File", NULL, 0, NULL, NULL, 0);
	if (!filepath_c_str) {
		return -1;
	}
	std::string filepath_str{ filepath_c_str };
	std::ifstream ifs{ filepath_str };
	serializer.load(entt_registry, ifs);

	entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader> mesh_cache{};
	entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> shader_cache{};

	Engine::script_load_system(entt_registry, sol_state);
	Engine::mesh_load_system(entt_registry, mesh_cache);
	Engine::shader_load_system(entt_registry, shader_cache);
	Engine::physics_body_load_system(entt_registry, physics_world);
	Engine::script_system_init(entt_registry);
	

	while (window.is_running()) {
		double prev_time = glfwGetTime();
		Engine::script_system_tick(entt_registry, sol_state);
		Engine::physics_body_system_tick(entt_registry);
		physics_world.tick(1.0f / 60.0f);
		renderer.render();
		double current_time = glfwGetTime();
		double delta = current_time - prev_time;
	}
}