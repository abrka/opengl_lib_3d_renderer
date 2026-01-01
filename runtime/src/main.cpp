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
#include "engine/systems/script_system.h"
#include "input/input.h"
#include "reflect/reflect.h"


int main() {
	sol::state sol_state{};
	sol_state.open_libraries(sol::lib::base, sol::lib::package);
	Reflect::register_sol_usertypes(sol_state);

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
	std::string filepath_str{ filepath_c_str };
	std::ifstream ifs{ filepath_str };
	serializer.load(entt_registry, ifs);

	entt::resource_cache<AssetBuilder::Scene, Engine::MeshLoader> mesh_cache{};
	entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> shader_cache{};

	while (window.is_running()) {
		double prev_time = glfwGetTime();

		Engine::script_load_system(entt_registry, sol_state);
		Engine::mesh_load_system(entt_registry, mesh_cache);
		Engine::shader_load_system(entt_registry, shader_cache);
		Engine::script_system_tick(entt_registry, sol_state);
		renderer.render();
		double current_time = glfwGetTime();
		double delta = current_time - prev_time;
	}
}