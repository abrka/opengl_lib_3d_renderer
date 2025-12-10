#include "renderer/renderer.h"
#include "nodes/mesh.h"
#include "editor/hierarchial_panel.h"

static float mouse_sensitivity = 0.005f;
static float cam_speed = 0.02f;

static void process_input_for_camera_movement(GLFWwindow* window, Camera& cam);

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
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

std::vector<Engine::Mesh*> get_all_mesh_nodes(Engine::Node& node) {
	std::vector<Engine::Mesh*> all_mesh_nodes{};
	Engine::Mesh* mesh_node = dynamic_cast<Engine::Mesh*>(&node);
	if (mesh_node) {
		all_mesh_nodes.push_back(mesh_node);
	}
	for (size_t i = 0; i < node.children.size(); i++)
	{
		const auto child_mesh_nodes = get_all_mesh_nodes(*node.children[i]);
		all_mesh_nodes.insert(all_mesh_nodes.end(), child_mesh_nodes.begin(), child_mesh_nodes.end());
	}
	return all_mesh_nodes;
}



int main() {
	const std::string asset_dir = std::string(TOSTRING(ASSET_DIR)) + "/";
	auto window = std::make_shared<GLExternalRAII::Window>(800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
	auto renderer = std::make_shared<Renderer>(window);


	std::shared_ptr<GL3D::ShaderProgram> pbr_shader = GLRenderer::ShaderBuilder::build(asset_dir + "shaders/pbr_frag.glsl", asset_dir + "shaders/pbr_vertex.glsl").value();

	std::shared_ptr<MeshBuilder::Scene> backpack_scene = MeshBuilder::build(asset_dir + "meshes/backpack/backpack.obj").value();
	std::shared_ptr<MeshBuilder::Scene> candle_scene = MeshBuilder::build(asset_dir + "meshes/candle/brass_candleholders_1k.gltf").value();
	std::shared_ptr<MeshBuilder::Scene> military_uniform_scene = MeshBuilder::build(asset_dir + "meshes/military_uniform/military_uniform.gltf").value();
	std::shared_ptr<MeshBuilder::Scene> sponza_scene = MeshBuilder::build(asset_dir + "meshes/sponza_palace/scene.gltf").value();

	auto mesh_node_1 = std::make_unique<Engine::Mesh>();
	mesh_node_1->name = "candle";
	mesh_node_1->scene = candle_scene;
	mesh_node_1->shader = pbr_shader;

	auto mesh_node_2 = std::make_unique<Engine::Mesh>();
	mesh_node_2->name = "backpack";
	mesh_node_2->scene = backpack_scene;
	mesh_node_2->shader = pbr_shader;
	mesh_node_2->transform = glm::scale(glm::mat4(1.0f), { 0.1,0.1,-0.1 });


	auto mesh_node_3 = std::make_unique<Engine::Mesh>();
	mesh_node_3->name = "military uniform";
	mesh_node_3->scene = military_uniform_scene;
	mesh_node_3->shader = pbr_shader;
	mesh_node_3->transform = glm::scale(glm::mat4(1.0f), { 0.02,0.02,0.02 });

	auto mesh_node_sponza = std::make_unique<Engine::Mesh>();
	mesh_node_sponza->name = "sponza scene";
	mesh_node_sponza->scene = sponza_scene;
	mesh_node_sponza->shader = pbr_shader;
	mesh_node_sponza->transform = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), { 1,0,0 });

	auto root_node = std::make_shared<Engine::Node>();
	root_node->name = "root";
	root_node->children.push_back(std::move(mesh_node_1));
	root_node->children.push_back(std::move(mesh_node_2));
	root_node->children.push_back(std::move(mesh_node_3));
	root_node->children.push_back(std::move(mesh_node_sponza));

	renderer->root_node = root_node;
	renderer->render_ctx.cam.position = glm::vec3{ 0, 0, -1 };



	Editor::HierarchialPanel<Engine::Node> hierarchial_panel{};

	ImGuizmo::OPERATION imguizmo_operation{ ImGuizmo::OPERATION::UNIVERSAL };
	ImGuizmo::MODE imguizmo_mode{ ImGuizmo::MODE::LOCAL };

	renderer->custom_imgui_render_function = [&imguizmo_operation, &imguizmo_mode, &root_node, &hierarchial_panel](Renderer& renderer) {
		ImGui::ShowDemoWindow();

		ImGui::Begin("Nodes");
		hierarchial_panel.render(*root_node);
		ImGui::End();

		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());

		auto [screen_width, screen_height] = renderer.get_screen_width_and_height();
		ImGuizmo::SetRect(0, 0, screen_width, screen_height);

		auto selected_mesh_node = dynamic_cast<Engine::Mesh*>(hierarchial_panel.selected_node);
		if (!selected_mesh_node) {
			return;
		}

		glm::mat4& mesh_node_transform = selected_mesh_node->transform;
		auto& camera = renderer.render_ctx.cam;
		ImGuizmo::Manipulate(glm::value_ptr(camera.get_view_matrix()), glm::value_ptr(camera.get_projection_matrix()), imguizmo_operation, imguizmo_mode, glm::value_ptr(mesh_node_transform));

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