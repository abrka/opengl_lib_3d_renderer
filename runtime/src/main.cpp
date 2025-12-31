#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sol/sol.hpp>
#include "renderer/renderer.h"
#include "engine/components/components.h"
#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"
#include "engine/serialize/entt_registry_serializer.h"
#include "engine/systems/script_system.h"
#include "input/input.h"
#include "reflect/reflect.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)


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
	input_system.disable_cursor();

	Renderer::Renderer3D renderer{ window, entt_registry };
	window.framebuffer_size_callback = [&renderer](int width, int height) {
		renderer.on_window_resize(width, height);
	};

	Engine::EnttRegistrySerializer<cereal::XMLInputArchive,cereal::XMLOutputArchive> serializer{};
	std::ifstream ifs{ "scene.kasset" };
	serializer.load(entt_registry, ifs);

	while (window.is_running()) {
		double prev_time = glfwGetTime();
		Engine::script_system_tick(entt_registry, sol_state);
		renderer.render();
		double current_time = glfwGetTime();
		double delta = current_time - prev_time;
	}
}