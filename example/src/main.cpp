#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>

#include "renderer/renderer.h"

#include "engine/components/components.h"

#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"

#include "editor/editor.h"
#include "editor/reflected_func_templates.h"
#include "editor/imreflect_custom_types.h"


static bool cursor_state_changed = false;
static float mouse_sensitivity = 0.005f;
static float cam_speed = 0.02f;

static void process_input_for_camera_movement(GLFWwindow* window, Renderer::Camera& cam);
static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

template<typename Archive, typename Snapshot>
void snapshot_get_func_custom(Archive& archive, Snapshot& snapshot) {
	snapshot.template get<Engine::RootComponent>(archive);
	snapshot.template get<Engine::ParentComponent>(archive);
	snapshot.template get<Engine::ChildrenComponent>(archive);
	snapshot.template get<Engine::NameComponent>(archive);
	snapshot.template get<Engine::TransformComponent>(archive);
	snapshot.template get<Engine::CameraComponent>(archive);
}

int main() {
	using namespace entt::literals;
	entt::registry entt_registry{};
	Editor::register_component<Engine::NameComponent>("NameComponent");
	Editor::register_component<Engine::TransformComponent>("TransformComponent");
	Editor::register_component<Engine::CameraComponent>("CameraComponent");
	Editor::register_component<Engine::PointLightComponent>("PointLightComponent");

	const std::string asset_dir = std::string(TOSTRING(ASSET_DIR)) + "/";
	GLExternalRAII::Window window{ 800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR };
	glfwSetInputMode(window.glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// WARNING: set key callbacks before renderer calls imgui. otherwise keys inside imgui wont work.
	glfwSetKeyCallback(window.glfw_window, key_callback);

	Renderer::Renderer3D renderer{ window, entt_registry };
	glfwSetWindowUserPointer(window.glfw_window, &renderer);
	glfwSetFramebufferSizeCallback(window.glfw_window, framebuffer_size_callback);

	entt::resource_cache<GL3D::ShaderProgram, Engine::ShaderLoader> entt_shader_cache{};
	auto pbr_shader = entt_shader_cache.load("pbr"_hs, asset_dir + "shaders/pbr_frag.glsl", asset_dir + "shaders/pbr_vertex.glsl").first->second;


	entt::resource_cache<MeshBuilder::Scene, Engine::MeshLoader> entt_mesh_cache{};
	auto backpack_scene = entt_mesh_cache.load("backpack"_hs, asset_dir + "meshes/backpack/backpack.obj").first->second;
	auto candle_scene = entt_mesh_cache.load("candle"_hs, asset_dir + "meshes/candle/brass_candleholders_1k.gltf").first->second;
	auto military_uniform_scene = entt_mesh_cache.load("uniform"_hs, asset_dir + "meshes/military_uniform/military_uniform.gltf").first->second;
	auto sponza_scene = entt_mesh_cache.load("sponza"_hs, asset_dir + "meshes/sponza_palace/scene.gltf").first->second;


	entt::entity root_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(root_entity, "root");
	entt_registry.emplace<Engine::RootComponent>(root_entity);

	entt::entity candle_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(candle_entity, "candle");
	entt_registry.emplace<Engine::MeshComponent>(candle_entity, candle_scene);
	entt_registry.emplace<Engine::ShaderComponent>(candle_entity, pbr_shader);
	entt_registry.emplace<Engine::TransformComponent>(candle_entity);
	Engine::add_child(entt_registry, root_entity, candle_entity);

	entt::entity backpack_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(backpack_entity, "backpack");
	entt_registry.emplace<Engine::MeshComponent>(backpack_entity, backpack_scene);
	entt_registry.emplace<Engine::ShaderComponent>(backpack_entity, pbr_shader);
	glm::mat4 backpack_transform = glm::scale(glm::mat4(1.0f), { 0.1,0.1,-0.1 });
	entt_registry.emplace<Engine::TransformComponent>(backpack_entity, backpack_transform);
	Engine::add_child(entt_registry, root_entity, backpack_entity);

	entt::entity military_uniform_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(military_uniform_entity, "military uniform");
	entt_registry.emplace<Engine::MeshComponent>(military_uniform_entity, military_uniform_scene);
	entt_registry.emplace<Engine::ShaderComponent>(military_uniform_entity, pbr_shader);
	glm::mat4 military_uniform_transform = glm::scale(glm::mat4(1.0f), { 0.02,0.02,0.02 });
	entt_registry.emplace<Engine::TransformComponent>(military_uniform_entity, military_uniform_transform);
	Engine::add_child(entt_registry, root_entity, military_uniform_entity);

	auto sponza_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(sponza_entity, "sponza");
	entt_registry.emplace<Engine::MeshComponent>(sponza_entity, sponza_scene);
	entt_registry.emplace<Engine::ShaderComponent>(sponza_entity, pbr_shader);
	glm::mat4 sponza_tranform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), { 1,0,0 });
	entt_registry.emplace<Engine::TransformComponent>(sponza_entity, sponza_tranform);
	Engine::add_child(entt_registry, root_entity, sponza_entity);

	entt::entity camera_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(camera_entity, "camera");
	Engine::CameraComponent camera_component{};
	camera_component.camera.position = { 0, 0, -1 };
	entt_registry.emplace<Engine::CameraComponent>(camera_entity, camera_component);
	Engine::add_child(entt_registry, root_entity, camera_entity);

	entt::entity point_light_entity = entt_registry.create();
	entt_registry.emplace<Engine::NameComponent>(point_light_entity, "point light");
	Engine::PointLightComponent point_light_comp{};
	point_light_comp.light.ambient_strength = 0.1f;
	point_light_comp.light.color = glm::vec3(1.0f);
	entt_registry.emplace<Engine::PointLightComponent>(point_light_entity, point_light_comp);
	entt_registry.emplace<Engine::TransformComponent>(point_light_entity);
	Engine::add_child(entt_registry, root_entity, point_light_entity);


	Engine::EnttRegistrySerializer<cereal::XMLInputArchive, cereal::XMLOutputArchive> serializer{
		&snapshot_get_func_custom<cereal::XMLOutputArchive, entt::snapshot>,
		&snapshot_get_func_custom<cereal::XMLInputArchive, entt::snapshot_loader>
	};
	Editor::Editor<cereal::XMLInputArchive, cereal::XMLOutputArchive> editor{ renderer, entt_registry, serializer};


	renderer.custom_imgui_render_function = [&editor](Renderer::Renderer3D& renderer) {
		editor.render();
	};

	while (window.is_running()) {
		auto entt_view_camera = entt_registry.view<const Engine::CameraComponent>();
		entt::entity camera_entity = entt_view_camera.front();
		auto* camera = renderer.get_camera();
		assert(camera && "No entity contains a CameraComponent");
		process_input_for_camera_movement(window.glfw_window, *camera);
		double prev_time = glfwGetTime();
		renderer.render();
		double delta = glfwGetTime() - prev_time;

		double fps = 1 / delta;
		const double fps_set_title_delay = 0.5;
		static double last_time_fps_was_set{};
		if (glfwGetTime() - last_time_fps_was_set > fps_set_title_delay) {
			std::string title = "fps: " + std::to_string(fps);
			glfwSetWindowTitle(window.glfw_window, title.c_str());
			last_time_fps_was_set = glfwGetTime();
		}
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer::Renderer3D* renderer = static_cast<Renderer::Renderer3D*>(glfwGetWindowUserPointer(window));
	renderer->on_window_resize(width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_CAPS_LOCK && action == GLFW_PRESS) {
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