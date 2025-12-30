#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/components/components.h"
#include "engine/systems/script_system.h"
#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"
#include "reflect/reflect.h"
#include "editor/editor.h"
#include "renderer/renderer.h"
#include "input/input.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static bool cursor_state_changed = false;
static float mouse_sensitivity = 0.005f;
static float cam_speed = 0.02f;

static void process_input_for_camera_movement(GLFWwindow* window, Renderer::Camera& cam);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


int main() {
	sol::state sol_state{};
	sol_state.open_libraries(sol::lib::base, sol::lib::package);
	sol_state.new_usertype<Engine::NameComponent>("NameComponent",
		"name", &Engine::NameComponent::name);

	entt::registry entt_registry{};
	Reflect::register_all_components();
	
	GLExternalRAII::Window window{ 800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR };
	window.key_callback = [&window](int key, int scancode, int action, int mods) {
		key_callback(window.glfw_window, key, scancode, action, mods);
	};

	Input::Input input_system{ *window.glfw_window };
	input_system.disable_cursor();

	Renderer::Renderer3D renderer{ window, entt_registry };
	window.framebuffer_size_callback = [&renderer](int width, int height) {
		renderer.on_window_resize(width, height);
	};

	entt::entity root_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(root_entity, "root");
	entt_registry.emplace<Engine::RootComponent>(root_entity);

	entt::entity camera_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(camera_entity, "camera");
	Engine::CameraComponent camera_component{};
	camera_component.camera.position = { 0, 0, -1 };
	entt_registry.emplace<Engine::CameraComponent>(camera_entity, camera_component);
	Engine::add_child(entt_registry, root_entity, camera_entity);

	Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer{};
	Editor::Editor3D editor{entt_registry,sol_state, serializer};

	renderer.custom_imgui_render_function = [&editor](Renderer::Renderer3D& renderer) {
		editor.render();
	};

	while (window.is_running()) {
		double prev_time = glfwGetTime();

		auto entt_view_camera = entt_registry.view<Engine::CameraComponent>();
		for (auto [entity, camera_component] : entt_view_camera.each()) {
			process_input_for_camera_movement(window.glfw_window, camera_component.camera);
		}
		

		auto entt_view_meshes = entt_registry.view<Engine::MeshComponent>();
		for (auto [entity, mesh_component] : entt_view_meshes.each()) {
			if (!mesh_component.scene) {
				continue;
			}
			auto materials = mesh_component.scene->get_all_materials();
			for (AssetBuilder::Material* material : materials) {
				material->set_uniform("u_material.specular_alpha", 32.0f);
				material->set_uniform("u_material.ambient_strength", 1.0f);
				material->set_uniform("u_material.diffuse_strength", 1.0f);
				material->set_uniform("u_material.specular_strength", 1.0f);
			}
		}

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_LEFT_ALT && action == GLFW_PRESS) {
		int prev_cursor_mode = glfwGetInputMode(window, GLFW_CURSOR);
		int new_cursor_mode = (prev_cursor_mode == GLFW_CURSOR_NORMAL) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(window, GLFW_CURSOR, new_cursor_mode);
		cursor_state_changed = true;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void process_input_for_camera_movement(GLFWwindow* window, Renderer::Camera& cam)
{

	static bool first_time_being_called = true;

	static double xpos_prev{};
	static double ypos_prev{};
	static double xpos{};
	static double ypos{};

	glfwGetCursorPos(window, &xpos, &ypos);
	if (first_time_being_called || cursor_state_changed) {
		xpos_prev = xpos;
		ypos_prev = ypos;
	}
	// calculate 
	double del_x = xpos - xpos_prev;
	double rot_y = del_x * mouse_sensitivity;

	static double cam_rot_x{};
	static double cam_rot_y{};

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		cam_rot_y -= rot_y;
	}

	double del_y = ypos - ypos_prev;
	double rot_x = del_y * mouse_sensitivity;

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		cam_rot_x += rot_x;
		cam_rot_x = glm::clamp(cam_rot_x, glm::radians(-89.0), glm::radians(89.0));
	}
	// calculate finished
	xpos_prev = xpos;
	ypos_prev = ypos;

	glm::vec3 cam_right = cam.orientation[0];
	glm::vec3 cam_up = cam.orientation[1];
	glm::vec3 cam_forward = cam.orientation[2];


	cam.orientation = glm::rotate(glm::mat4(1.0f), (float)cam_rot_y, glm::vec3(0.0f, 1.0f, 0.0f));
	cam.orientation = glm::rotate(cam.orientation, (float)cam_rot_x, glm::vec3(1.0f, 0.0f, 0.0f));

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cam.position += cam_right * cam_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cam.position -= cam_right * cam_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cam.position += cam_forward * cam_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cam.position -= cam_forward * cam_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			cam.position -= cam_up * cam_speed;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			cam.position += cam_up * cam_speed;
		}
	}
	first_time_being_called = false;
	cursor_state_changed = false;
}