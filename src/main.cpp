#include "renderer.h"


static float mouse_sensitivity = 0.005;
static float camSpeed = 0.02;

static void process_input(GLFWwindow* window, Camera& cam);

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
	renderer->on_window_resize(width, height);
}

int main() {
	auto window = std::make_shared<GLExternalRAII::Window>(800, 800, OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR);
	auto renderer = std::make_shared<Renderer>(window);
	
	glfwSetInputMode(window->glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window->glfw_window, renderer.get());
	glfwSetFramebufferSizeCallback(window->glfw_window, framebuffer_size_callback);

	renderer->cam.position = glm::vec3{ 0, 0, -1 };

	while (window->is_running()) {
		process_input(window->glfw_window, renderer->cam);
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
static void process_input(GLFWwindow* window, Camera& cam)
{
	static bool first_time_being_called = true;
	static bool cursor_state_changed = false;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		cursor_state_changed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursor_state_changed = true;
	}


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
	float del_x = xpos - xpos_prev;
	float rot_y = del_x * mouse_sensitivity;

	static float camRotX{};
	static float camRotY{};

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		camRotY -= rot_y;
	}

	float del_y = ypos - ypos_prev;
	float rot_x = del_y * mouse_sensitivity;

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		camRotX += rot_x;
		camRotX = std::clamp(camRotX, glm::radians(-89.0f), glm::radians(89.0f));
	}
	// calculate finished
	xpos_prev = xpos;
	ypos_prev = ypos;

	glm::vec3 camRight = cam.orientation[0];
	glm::vec3 camUp = cam.orientation[1];
	glm::vec3 camForward = cam.orientation[2];
	

	cam.orientation = glm::rotate(glm::mat4(1.0f), camRotY, glm::vec3(0.0f, 1.0f, 0.0f));
	cam.orientation = glm::rotate(cam.orientation, camRotX, glm::vec3(1.0f, 0.0f, 0.0f));

	if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_NORMAL) {
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			cam.position += camRight * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			cam.position -= camRight * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			cam.position += camForward * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			cam.position -= camForward * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			cam.position -= camUp * camSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			cam.position += camUp * camSpeed;
		}
	}


	first_time_being_called = false;
	cursor_state_changed = false;
}