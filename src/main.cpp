#include <entt/entt.hpp>

#include "renderer/renderer.h"
#include "engine/entity/entity.h"
#include "editor/hierarchical_panel.h"
#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"

static float mouse_sensitivity = 0.005f;
static float cam_speed = 0.02f;

static void process_input_for_camera_movement(GLFWwindow* window, Camera& cam);

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer::Renderer3D* renderer = static_cast<Renderer::Renderer3D*>(glfwGetWindowUserPointer(window));
	renderer->on_window_resize(width, height);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS) {
		int prev_cursor_mode = glfwGetInputMode(window, GLFW_CURSOR);
		int new_cursor_mode = (prev_cursor_mode == GLFW_CURSOR_NORMAL) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
		glfwSetInputMode(window, GLFW_CURSOR, new_cursor_mode);
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}


int main() {
	entt::registry entt_registry{};

	const std::string asset_dir = std::string(TOSTRING(ASSET_DIR)) + "/";
	auto window = std::make_shared<GLExternalRAII::Window>(800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
	auto renderer = std::make_shared<Renderer::Renderer3D>(window, entt_registry);

	using namespace entt::literals;

	entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> entt_shader_cache{};
	auto pbr_shader = entt_shader_cache.load("pbr"_hs, asset_dir + "shaders/pbr_frag.glsl", asset_dir + "shaders/pbr_vertex.glsl").first->second;

	
	entt::resource_cache<MeshBuilder::Scene, Engine::MeshLoader> entt_mesh_cache{};
	auto backpack_scene = entt_mesh_cache.load("backpack"_hs, asset_dir + "meshes/backpack/backpack.obj").first->second;
	auto candle_scene = entt_mesh_cache.load("candle"_hs, asset_dir + "meshes/candle/brass_candleholders_1k.gltf").first->second;
	auto military_uniform_scene = entt_mesh_cache.load("uniform"_hs, asset_dir + "meshes/military_uniform/military_uniform.gltf").first->second;
	auto sponza_scene = entt_mesh_cache.load("sponza"_hs, asset_dir + "meshes/sponza_palace/scene.gltf").first->second;

	auto candle_entity = std::make_unique<Engine::Entity>(entt_registry);
	candle_entity->name = "candle";
	candle_entity->add_component<Engine::MeshComponent>(candle_scene);
	candle_entity->add_component<Engine::ShaderComponent>(pbr_shader);
	candle_entity->add_component<Engine::TransformComponent>(glm::mat4(1.0f));

	auto backpack_entity = std::make_unique<Engine::Entity>(entt_registry);
	backpack_entity->name = "backpack";
	backpack_entity->add_component<Engine::MeshComponent>(backpack_scene);
	backpack_entity->add_component<Engine::ShaderComponent>(pbr_shader);
	glm::mat4 backpack_transform = glm::scale(glm::mat4(1.0f), { 0.1,0.1,-0.1 });
	backpack_entity->add_component<Engine::TransformComponent>(backpack_transform);
	
	
	auto military_uniform_entity = std::make_unique<Engine::Entity>(entt_registry);
	military_uniform_entity->name = "military uniform";
	military_uniform_entity->add_component<Engine::MeshComponent>(military_uniform_scene);
	military_uniform_entity->add_component<Engine::ShaderComponent>(pbr_shader);
	glm::mat4 military_uniform_transform = glm::scale(glm::mat4(1.0f), { 0.02,0.02,0.02 });
	military_uniform_entity->add_component<Engine::TransformComponent>(military_uniform_transform);
	
	auto sponza_entity = std::make_unique<Engine::Entity>(entt_registry);
	sponza_entity->name = "sponza scene";
	sponza_entity->add_component<Engine::MeshComponent>(sponza_scene);
	sponza_entity->add_component<Engine::ShaderComponent>(pbr_shader);
	glm::mat4 sponza_tranform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), { 1,0,0 });
	sponza_entity->add_component<Engine::TransformComponent>(sponza_tranform);

	auto root_entity = std::make_unique<Engine::Entity>(entt_registry);
	root_entity->name = "root";
	root_entity->add_child(std::move(candle_entity));
	root_entity->add_child(std::move(backpack_entity));
	root_entity->add_child(std::move(military_uniform_entity));
	root_entity->add_child(std::move(sponza_entity));


	renderer->render_ctx.cam.position = glm::vec3{ 0, 0, -1 };



	Editor::HierarchicalPanel<Engine::Entity> hierarchical_panel{};

	ImGuizmo::OPERATION imguizmo_operation{ ImGuizmo::OPERATION::UNIVERSAL };
	ImGuizmo::MODE imguizmo_mode{ ImGuizmo::MODE::LOCAL };

	renderer->custom_imgui_render_function = [&imguizmo_operation, &imguizmo_mode, &root_entity, &hierarchical_panel](Renderer::Renderer3D& renderer) {
		ImGui::ShowDemoWindow();

		ImGui::Begin("Nodes");
		hierarchical_panel.render(*root_entity);
		ImGui::End();

		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

		auto [screen_width, screen_height] = renderer.get_screen_width_and_height();
		ImGuizmo::SetRect(0, 0, screen_width, screen_height);

		Engine::Entity* selected_entity = hierarchical_panel.selected_node;
		if (!selected_entity) {
			return;
		}
		glm::mat4* transform = selected_entity->try_get_component<Engine::TransformComponent>();
		if (!transform) {
			return;
		}
		auto& camera = renderer.render_ctx.cam;
		ImGuizmo::Manipulate(glm::value_ptr(camera.get_view_matrix()), glm::value_ptr(camera.get_projection_matrix()), imguizmo_operation, imguizmo_mode, glm::value_ptr(*transform));

		};


	glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window->glfw_window, renderer.get());
	glfwSetFramebufferSizeCallback(window->glfw_window, framebuffer_size_callback);
	glfwSetKeyCallback(window->glfw_window, key_callback);

	while (window->is_running()) {
		process_input_for_camera_movement(window->glfw_window, renderer->render_ctx.cam);
		double prev_time = glfwGetTime();
		renderer->render();
		double delta = glfwGetTime() - prev_time;
		double fps = 1 / delta;
		const double fps_set_title_delay = 0.5;
		static double last_time_fps_was_set{};
		if (glfwGetTime() - last_time_fps_was_set > fps_set_title_delay) {
			std::string title = "fps: " + std::to_string(fps);
			glfwSetWindowTitle(window->glfw_window, title.c_str());
			last_time_fps_was_set = glfwGetTime();
		}
	}
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void process_input_for_camera_movement(GLFWwindow* window, Camera& cam)
{

	static bool first_time_being_called = true;

	static double xpos_prev{};
	static double ypos_prev{};
	static double xpos{};
	static double ypos{};

	glfwGetCursorPos(window, &xpos, &ypos);
	if (first_time_being_called) {
		xpos_prev = xpos;
		ypos_prev = ypos;
	}
	// calculate 
	float del_x = xpos - xpos_prev;
	float rot_y = del_x * mouse_sensitivity;

	static float cam_rot_x{};
	static float cam_rot_y{};

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		cam_rot_y -= rot_y;
	}

	float del_y = ypos - ypos_prev;
	float rot_x = del_y * mouse_sensitivity;

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		cam_rot_x += rot_x;
		cam_rot_x = glm::clamp(cam_rot_x, glm::radians(-89.0f), glm::radians(89.0f));
	}
	// calculate finished
	xpos_prev = xpos;
	ypos_prev = ypos;

	glm::vec3 cam_right = cam.orientation[0];
	glm::vec3 cam_up = cam.orientation[1];
	glm::vec3 cam_forward = cam.orientation[2];


	cam.orientation = glm::rotate(glm::mat4(1.0f), cam_rot_y, glm::vec3(0.0f, 1.0f, 0.0f));
	cam.orientation = glm::rotate(cam.orientation, cam_rot_x, glm::vec3(1.0f, 0.0f, 0.0f));

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
}